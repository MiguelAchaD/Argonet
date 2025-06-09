#pragma once

#include "Pool.hpp"
#include "Accepter.hpp"
#include "Resolver.hpp"
#include "Forwarder.hpp"
#include "Sender.hpp"
#include "Logger.hpp"
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

namespace proxyServer {

class Handler {
public:
    Handler(size_t num_threads = 4);
    ~Handler();

    void start(size_t num_threads);
    void stop();
    void handlePacket(petitionPacket packet);
    bool isRunning() const;

private:
    void workerThread();
    void processPacket(petitionPacket& packet);
    void logPoolCounts() const;

    Pool<Accepter, true> m_accepter_pool;
    Pool<Resolver, false> m_resolver_pool;
    Pool<Forwarder, true> m_forwarder_pool;
    Pool<Sender, true> m_sender_pool;

    std::vector<std::thread> m_worker_threads;
    std::queue<petitionPacket> m_work_queue;
    std::mutex m_queue_mutex;
    std::condition_variable m_queue_cv;
    std::atomic<bool> m_running{false};

};

} // namespace proxyServer
