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
    std::vector<std::unique_ptr<T>> m_pool_busy;

    void makeAvailable(std::unique_ptr<T>& t_object) {
        auto it = std::find_if(m_pool_busy.begin(), m_pool_busy.end(), 
            [&t_object](const std::unique_ptr<T>& uptr) { return uptr.get() == t_object.get(); });

        if (it != m_pool_busy.end()) {
            m_pool.push_back(std::move(*it));
            m_pool_busy.erase(it);
        }
    }

public:
    bool addToPool(std::unique_ptr<T> t_object) {
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
            [&t_object](const std::unique_ptr<T>& obj) { return obj.get() == &t_object; });

        if (it != m_pool.end()) {
            m_pool.erase(it);
            return true;
        }
        return false;
    }

    proxyServer::petitionPacket poolInvokePetitionPacketInitial(unsigned short int t_client_socket) {
        if (m_pool.empty()) {
            return {};
        }

        std::unique_ptr<T> obj = std::move(m_pool.back());
            // std::cout << "sigue...poolInvokePetitionPacketInitial\n";
        m_pool.pop_back();
        
        if (!obj) {
            return {};
        }

        proxyServer::petitionPacket result = obj->execute(t_client_socket);

        m_pool.push_back(std::move(obj));
            // std::cout << "sigue...poolInvokePetitionPacketInitial\n";

        return result;
    }

    proxyServer::petitionPacket poolInvokePetitionPacket(proxyServer::petitionPacket t_packet) {
        if (m_pool.empty()) {
            return {};
        }

        std::unique_ptr<T> obj = std::move(m_pool.back());
            // std::cout << "sigue...poolInvokePetitionPacket\n";
        m_pool.pop_back();
        
        if (!obj) {
            return {};
        }

        proxyServer::petitionPacket result = obj->execute(t_packet);

        m_pool.push_back(std::move(obj));
            // std::cout << "sigue...poolInvokePetitionPacket\n";

        return result;
    }

    void poolInvokeVoid(proxyServer::petitionPacket t_packet) {
        if (m_pool.empty()) {
            return;
        }

        std::unique_ptr<T> obj = std::move(m_pool.back());
            // std::cout << "sigue...poolInvokeVoid\n";
        m_pool.pop_back();
        
        if (!obj) {
            return;
        }

        obj->execute(t_packet);

        m_pool.push_back(std::move(obj));
            // std::cout << "sigue...poolInvokeVoid\n";
    }

    std::unique_ptr<T> poolInvoke() {
        if (!m_pool.empty()) {
            std::unique_ptr<T> obj = std::move(m_pool.back());
            m_pool.pop_back();
            m_pool_busy.push_back(std::move(obj));
            return std::move(m_pool_busy.back());
        }
        return nullptr;
    }

    void poolRevoke(std::unique_ptr<T> t_object) {
        makeAvailable(t_object);
    }
};
