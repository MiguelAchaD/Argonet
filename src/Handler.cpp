#include "Handler.hpp"
#include "Logger.hpp"
#include "Command.hpp"
#include "Server.hpp"
#include <stdexcept>
#include <sstream>
#include <algorithm>

namespace proxyServer {

Handler::Handler(size_t num_threads) 
    : m_accepter_pool(2, 4, nullptr)
    , m_resolver_pool(2, 4, nullptr)
    , m_forwarder_pool(2, 4, nullptr)
    , m_sender_pool(2, 4, nullptr)
{
    try {
        // Read initial configuration
        nlohmann::json config = Command::readConfigurationFile();
        if (!config.contains("pools")) {
            throw std::runtime_error("Configuration missing pools section");
        }

        const auto& pools = config["pools"];
        
        // Update pool configurations
        m_accepter_pool.updateLimits(
            pools["accepter"]["initial"].get<size_t>(),
            pools["accepter"]["max"].get<size_t>()
        );
        
        m_resolver_pool.updateLimits(
            pools["resolver"]["initial"].get<size_t>(),
            pools["resolver"]["max"].get<size_t>()
        );
        
        m_forwarder_pool.updateLimits(
            pools["forwarder"]["initial"].get<size_t>(),
            pools["forwarder"]["max"].get<size_t>()
        );
        
        m_sender_pool.updateLimits(
            pools["sender"]["initial"].get<size_t>(),
            pools["sender"]["max"].get<size_t>()
        );

        logPoolCounts();
        start(num_threads);
    } catch (const std::exception& e) {
        Logger::log("Failed to initialize Handler: " + std::string(e.what()), 
                   Logger::LogType::ERROR);
        throw;
    }
}

Handler::~Handler() {
    stop();
}

void Handler::logPoolCounts() const {
    Logger::log("Pools configuration updated successfully.", Logger::LogType::SUCCESS);
    std::stringstream ss;
    ss << "Pool counts - Sender: " << m_sender_pool.getTotalCount()
       << ", Forwarder: " << m_forwarder_pool.getTotalCount()
       << ", Resolver: " << m_resolver_pool.getTotalCount()
       << ", Accepter: " << m_accepter_pool.getTotalCount();
    Logger::log(ss.str(), Logger::LogType::LOG);
}

void Handler::pauseProcessing() {
    std::lock_guard<std::mutex> lock(m_config_mutex);
    m_paused = true;
    // Wait for current processing to complete
    while (!m_work_queue.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

void Handler::resumeProcessing() {
    std::lock_guard<std::mutex> lock(m_config_mutex);
    m_paused = false;
    m_queue_cv.notify_all();
}

void Handler::updatePoolConfiguration(const nlohmann::json& config) {
    try {
        if (!config.contains("pools")) {
            throw std::runtime_error("Configuration missing pools section");
        }

        // Pause processing to prevent race conditions
        pauseProcessing();

        const auto& pools = config["pools"];
        const std::vector<std::string> pool_types = {"accepter", "forwarder", "resolver", "sender"};

        for (const auto& pool_type : pool_types) {
            if (!pools.contains(pool_type)) {
                throw std::runtime_error("Missing configuration for pool: " + pool_type);
            }

            const auto& pool_config = pools[pool_type];
            if (!pool_config.contains("initial") || !pool_config.contains("max")) {
                throw std::runtime_error("Invalid configuration for pool: " + pool_type);
            }

            size_t initial = pool_config["initial"].get<size_t>();
            size_t max = pool_config["max"].get<size_t>();

            if (initial > max) {
                throw std::runtime_error("Initial size cannot be greater than max size for pool: " + pool_type);
            }

            // Update the appropriate pool
            if (pool_type == "accepter") {
                m_accepter_pool.updateLimits(initial, max);
            } else if (pool_type == "forwarder") {
                m_forwarder_pool.updateLimits(initial, max);
            } else if (pool_type == "resolver") {
                m_resolver_pool.updateLimits(initial, max);
            } else if (pool_type == "sender") {
                m_sender_pool.updateLimits(initial, max);
            }
        }

        logPoolCounts();

        // Resume processing
        resumeProcessing();
    } catch (const std::exception& e) {
        Logger::log("Failed to update pool configurations: " + std::string(e.what()), 
                   Logger::LogType::ERROR);
        resumeProcessing();
        throw;
    }
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

    // Start scaling thread
    m_last_scaling_check = std::chrono::steady_clock::now();
    m_scaling_thread = std::thread(&Handler::scalingThread, this);
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

    if (m_scaling_thread.joinable()) {
        m_scaling_thread.join();
    }
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
            m_queue_cv.wait(lock, [this] { 
                return !m_work_queue.empty() || !m_running || m_paused; 
            });
            
            if (!m_running || (m_paused && m_work_queue.empty())) {
                return;
            }
            
            if (!m_work_queue.empty()) {
                packet = std::move(m_work_queue.front());
                m_work_queue.pop();
            }
        }

        if (!m_paused) {
            processPacket(packet);
        }
    }
}

void Handler::scalingThread() {
    while (m_running) {
        std::this_thread::sleep_for(std::chrono::seconds(5)); // Check every 5 seconds

        if (!m_running) break;

        // Update pool sizes based on workload
        updatePoolSize(m_accepter_pool, "accepter");
        updatePoolSize(m_resolver_pool, "resolver");
        updatePoolSize(m_forwarder_pool, "forwarder");
        updatePoolSize(m_sender_pool, "sender");

        // Reset metrics
        {
            std::lock_guard<std::mutex> lock(m_metrics_mutex);
            m_processing_times.clear();
            m_peak_queue_size = 0;
        }
        m_total_processed = 0;
    }
}

void Handler::processPacket(petitionPacket& packet) {
    try {
        auto start_time = std::chrono::steady_clock::now();

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
                    
                    // Decrement active connections after sender finishes
                    if (m_server) {
                        m_server->decrementActiveConnections();
                    }
                }
            }
        }

        // Update metrics
        {
            std::lock_guard<std::mutex> lock(m_metrics_mutex);
            m_processing_times.push_back(start_time);
            while (m_processing_times.size() > METRICS_WINDOW) {
                m_processing_times.pop_front();
            }
        }
        m_total_processed++;

        // Update peak queue size
        {
            std::lock_guard<std::mutex> lock(m_queue_mutex);
            size_t current_size = m_work_queue.size();
            size_t current_peak = m_peak_queue_size.load();
            if (current_size > current_peak) {
                m_peak_queue_size.store(current_size);
            }
        }

    } catch (const std::exception& e) {
        Logger::log("Error processing packet: " + std::string(e.what()), Logger::LogType::ERROR);
        // Also decrement on error to prevent connection count from growing
        if (m_server) {
            m_server->decrementActiveConnections();
        }
    }
}

size_t Handler::getWorkerCount(const std::string& pool_type) const {
    if (pool_type == "accepter") {
        return m_accepter_pool.getTotalCount();
    } else if (pool_type == "resolver") {
        return m_resolver_pool.getTotalCount();
    } else if (pool_type == "forwarder") {
        return m_forwarder_pool.getTotalCount();
    } else if (pool_type == "sender") {
        return m_sender_pool.getTotalCount();
    }
    return 0;
}

} // namespace proxyServer
