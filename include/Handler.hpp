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
#include <chrono>
#include <deque>

namespace proxyServer {

// Forward declaration
class Server;

class Handler {
public:
    Handler(size_t num_threads = 4);
    ~Handler();

    void start(size_t num_threads);
    void stop();
    void handlePacket(petitionPacket packet);
    bool isRunning() const;
    void updatePoolConfiguration(const nlohmann::json& config);
    size_t getWorkerCount(const std::string& pool_type) const;
    void setServer(Server* server) { m_server = server; }

private:
    void workerThread();
    void processPacket(petitionPacket& packet);
    void logPoolCounts() const;
    void pauseProcessing();
    void resumeProcessing();
    void scalingThread();

    template<typename T, bool B>
    void updatePoolSize(Pool<T, B>& pool, const std::string& pool_name) {
        size_t current_size = pool.getTotalCount();
        size_t min_size = pool.getMinSize();
        size_t max_size = pool.getMaxSize();
        
        // Calculate average processing time
        double avg_processing_time = 0;
        {
            std::lock_guard<std::mutex> lock(m_metrics_mutex);
            if (!m_processing_times.empty()) {
                auto now = std::chrono::steady_clock::now();
                auto oldest = m_processing_times.front();
                avg_processing_time = std::chrono::duration<double>(now - oldest).count() / m_processing_times.size();
            }
        }

        // Calculate requests per second over a shorter window for faster response
        double requests_per_second = m_total_processed / 5.0; // Over the last 5 seconds

        // Calculate queue utilization
        size_t current_queue_size;
        {
            std::lock_guard<std::mutex> lock(m_queue_mutex);
            current_queue_size = m_work_queue.size();
        }
        double queue_utilization = static_cast<double>(current_queue_size) / m_peak_queue_size;

        // Decision logic for scaling with more aggressive scale-up
        bool should_increase = false;
        bool should_decrease = false;
        bool emergency_scale = false;

        if (current_size < max_size) {
            // Emergency scale-up if queue is getting too large
            emergency_scale = current_queue_size > 50 || queue_utilization > 0.9;

            // Normal scale-up conditions
            should_increase = (queue_utilization > 0.5 && avg_processing_time > 0.1) || 
                            (requests_per_second > 10 && avg_processing_time > 0.05) ||
                            emergency_scale;
        }

        if (current_size > min_size) {
            // More conservative scale-down
            should_decrease = (queue_utilization < 0.1 && avg_processing_time < 0.02) || 
                            (requests_per_second < 2 && avg_processing_time < 0.01);
        }

        if (should_increase) {
            size_t new_size;
            if (emergency_scale) {
                // In emergency, scale up more aggressively
                new_size = std::min(current_size + 2, max_size);
            } else {
                new_size = std::min(current_size + 1, max_size);
            }
            pool.updateLimits(min_size, new_size);
            Logger::log("Increased " + pool_name + " pool size to " + std::to_string(new_size), 
                       Logger::LogType::LOG);
        } else if (should_decrease) {
            size_t new_size = std::max(current_size - 1, min_size);
            pool.updateLimits(min_size, new_size);
            Logger::log("Decreased " + pool_name + " pool size to " + std::to_string(new_size), 
                       Logger::LogType::LOG);
        }
    }

    // Worker pools
    Pool<Accepter, true> m_accepter_pool;
    Pool<Resolver, false> m_resolver_pool;
    Pool<Forwarder, true> m_forwarder_pool;
    Pool<Sender, true> m_sender_pool;

    // Thread management
    std::vector<std::thread> m_worker_threads;
    std::thread m_scaling_thread;
    std::queue<petitionPacket> m_work_queue;
    std::mutex m_queue_mutex;
    std::mutex m_config_mutex;
    std::condition_variable m_queue_cv;
    std::atomic<bool> m_running{false};
    std::atomic<bool> m_paused{false};
    Server* m_server{nullptr};

    // Scaling metrics
    static constexpr size_t METRICS_WINDOW = 60; // 1 minute window
    std::deque<std::chrono::steady_clock::time_point> m_processing_times;
    std::mutex m_metrics_mutex;
    std::atomic<size_t> m_total_processed{0};
    std::atomic<size_t> m_peak_queue_size{0};
    std::chrono::steady_clock::time_point m_last_scaling_check;
};

} // namespace proxyServer
