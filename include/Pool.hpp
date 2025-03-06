#pragma once

#include "Socket.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

template <typename T>
class Pool {
    private:
        std::vector<std::unique_ptr<T>> m_pool;

    public:
        bool addToPool(std::unique_ptr<T>&& t_object) {
            m_pool.push_back(std::move(t_object));
            return true;
        }

        bool addToPoolN(unsigned short int t_number) {
            for (size_t i = 0; i < t_number; i++) {
                m_pool.push_back(std::make_unique<T>());
            }
            return true;
        }

        bool removeFromPool(T& t_object) {
            auto it = std::find_if(m_pool.begin(), m_pool.end(), 
                [&t_object](const std::unique_ptr<T>& obj) {
                    return *obj == t_object;
                });
            if (it != m_pool.end()) {
                m_pool.erase(it);
                return true;
            }
            return false;
        }

        bool removeFromPoolN(unsigned short int t_number) {
            if (t_number > m_pool.size()) {
                return false;
            }
            m_pool.erase(m_pool.end() - t_number, m_pool.end());
            return true;
        }

        Pool() {}
        ~Pool() {
            m_pool.clear();
}
};

