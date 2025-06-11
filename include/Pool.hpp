#pragma once

#include <vector>
#include <memory>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <stdexcept>
#include <functional>
#include "Packet.hpp"
#include "Logger.hpp"

namespace proxyServer {

template<typename T, bool RequiresPort = false>
class Pool {
private:
    struct PoolObject {
        std::unique_ptr<T> object;
        std::atomic<bool> in_use{false};

        PoolObject() = default;
        PoolObject(PoolObject&& other) noexcept 
            : object(std::move(other.object)), in_use(other.in_use.load()) {}
        PoolObject& operator=(PoolObject&& other) noexcept {
            if (this != &other) {
                object = std::move(other.object);
                in_use.store(other.in_use.load());
            }
            return *this;
        }
    };

    std::vector<PoolObject> m_objects;
    std::mutex m_mutex;
    std::atomic<size_t> m_total_count{0};
    std::atomic<size_t> m_available_count{0};
    std::atomic<size_t> m_high_water_mark{0};
    const size_t m_initial_size;
    std::atomic<size_t> m_max_size;
    std::function<void()> m_on_object_created;

    unsigned short int pickPort() {
        static unsigned short int port = 8080;
        return port++;
    }

    void initializePool(size_t size) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_objects.reserve(size);
        for (size_t i = 0; i < size; ++i) {
            m_objects.emplace_back();
            if constexpr (RequiresPort) {
                m_objects.back().object = std::make_unique<T>(pickPort());
            } else {
                m_objects.back().object = std::make_unique<T>();
            }
            m_objects.back().in_use = false;
        }
        m_total_count = size;
        m_available_count = size;
    }

public:
    Pool(size_t initial_size = 10, size_t max_size = 100, 
         std::function<void()> on_object_created = nullptr)
        : m_initial_size(initial_size)
        , m_max_size(max_size)
        , m_on_object_created(std::move(on_object_created)) {
        initializePool(initial_size);
    }

    T* acquire() {
        std::lock_guard<std::mutex> lock(m_mutex);
        
        // Try to find an available object
        for (auto& obj : m_objects) {
            if (!obj.in_use) {
                obj.in_use = true;
                m_available_count--;
                m_high_water_mark = std::max(m_high_water_mark.load(), 
                                           m_total_count.load() - m_available_count.load());
                return obj.object.get();
            }
        }

        // If we haven't reached max size, create a new object
        if (m_total_count < m_max_size) {
            m_objects.emplace_back();
            if constexpr (RequiresPort) {
                m_objects.back().object = std::make_unique<T>(pickPort());
            } else {
                m_objects.back().object = std::make_unique<T>();
            }
            m_objects.back().in_use = true;
            m_total_count++;
            m_high_water_mark = std::max(m_high_water_mark.load(), 
                                       m_total_count.load() - m_available_count.load());
            
            // Notify that a new object was created
            if (m_on_object_created) {
                m_on_object_created();
            }
            
            return m_objects.back().object.get();
        }

        throw std::runtime_error("Pool exhausted");
    }

    void release(T* obj) {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (auto& pool_obj : m_objects) {
            if (pool_obj.object.get() == obj) {
                pool_obj.in_use = false;
                m_available_count++;
                return;
            }
        }
        throw std::runtime_error("Object not found in pool");
    }

    size_t getTotalCount() const { return m_total_count; }
    size_t getAvailableCount() const { return m_available_count; }
    size_t getHighWaterMark() const { return m_high_water_mark; }

    void updateLimits(size_t initial_size, size_t max_size) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_max_size = max_size;
        
        // Adjust pool size to match initial_size
        while (m_objects.size() < initial_size) {
            m_objects.emplace_back();
            if constexpr (RequiresPort) {
                m_objects.back().object = std::make_unique<T>(pickPort());
            } else {
                m_objects.back().object = std::make_unique<T>();
            }
            m_objects.back().in_use = false;
            m_total_count++;
            m_available_count++;
        }
        while (m_objects.size() > initial_size) {
            m_objects.pop_back();
            m_total_count--;
            m_available_count--;
        }
        
        if (m_on_object_created) {
            m_on_object_created();
        }
    }

    size_t getMinSize() const {
        return m_initial_size;
    }

    size_t getMaxSize() const {
        return m_max_size;
    }
};

} // namespace proxyServer
