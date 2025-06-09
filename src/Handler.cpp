#include "Handler.hpp"
#include "Logger.hpp"
#include <stdexcept>
#include <sstream>

namespace proxyServer {

Handler::Handler(size_t num_threads) 
    : m_accepter_pool(1, 100, [this]() { logPoolCounts(); })
    , m_resolver_pool(1, 100, [this]() { logPoolCounts(); })
    , m_forwarder_pool(1, 100, [this]() { logPoolCounts(); })
    , m_sender_pool(1, 100, [this]() { logPoolCounts(); }) {
    logPoolCounts();
    start(num_threads);
}

Handler::~Handler() {
    stop();
}

void Handler::logPoolCounts() const {
    std::stringstream ss;
    ss << "Pool counts - Sender: " << m_sender_pool.getTotalCount()
       << ", Forwarder: " << m_forwarder_pool.getTotalCount()
       << ", Resolver: " << m_resolver_pool.getTotalCount()
       << ", Accepter: " << m_accepter_pool.getTotalCount();
    Logger::log(ss.str(), Logger::LogType::LOG);
}

void Handler::start(size_t num_threads) {
    if (m_running) {
        return;
    }

    m_running = true;
    m_worker_threads.clear();
    m_worker_threads.reserve(num_threads);

    for (size_t i = 0; i < num_threads; ++i) {
        m_worker_threads.emplace_back(&Handler::workerThread, this);
    }
}

void Handler::stop() {
    if (!m_running) {
        return;
    }

    m_running = false;
    m_queue_cv.notify_all();

    for (auto& thread : m_worker_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    m_worker_threads.clear();
}

void Handler::handlePacket(petitionPacket packet) {
    {
        std::lock_guard<std::mutex> lock(m_queue_mutex);
        m_work_queue.push(std::move(packet));
    }
    m_queue_cv.notify_one();
}

bool Handler::isRunning() const {
    return m_running;
}

void Handler::workerThread() {
    while (m_running) {
        petitionPacket packet;
        {
            std::unique_lock<std::mutex> lock(m_queue_mutex);
            m_queue_cv.wait(lock, [this] { return !m_work_queue.empty() || !m_running; });
            if (!m_running && m_work_queue.empty()) {
                return;
            }
            packet = std::move(m_work_queue.front());
            m_work_queue.pop();
        }

        processPacket(packet);
    }
}

void Handler::processPacket(petitionPacket& packet) {
    try {
        // Process packet through pipeline
        auto* accepter = m_accepter_pool.acquire();
        packet = accepter->execute(packet.client_socket);
        m_accepter_pool.release(accepter);

        if (packet.client_socket > 0) {
            auto* resolver = m_resolver_pool.acquire();
            packet = resolver->execute(packet);
            m_resolver_pool.release(resolver);

            if (packet.client_socket > 0) {
                auto* forwarder = m_forwarder_pool.acquire();
                packet = forwarder->execute(packet);
                m_forwarder_pool.release(forwarder);

                if (packet.client_socket > 0) {
                    auto* sender = m_sender_pool.acquire();
                    sender->execute(packet);
                    m_sender_pool.release(sender);
                }
            }
        }
    } catch (const std::exception& e) {
        Logger::log("Error processing packet: " + std::string(e.what()), Logger::LogType::ERROR);
    }
}

} // namespace proxyServer
