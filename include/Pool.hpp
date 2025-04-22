#pragma once

<<<<<<< Updated upstream
<<<<<<< Updated upstream
=======
=======
<<<<<<< Updated upstream
<<<<<<< HEAD
=======
>>>>>>> Stashed changes
#include "Socket.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
=======
<<<<<<< Updated upstream
<<<<<<< Updated upstream
=======
>>>>>>> c0efdf229e92f4978f156818e4f77faacca9e41e
>>>>>>> Stashed changes
>>>>>>> Stashed changes
namespace proxyServer {
    class Pool {
    private:
        
<<<<<<< Updated upstream
>>>>>>> Stashed changes
=======
<<<<<<< Updated upstream
>>>>>>> Stashed changes

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
<<<<<<< Updated upstream
=======
=======
>>>>>>> Stashed changes

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
<<<<<<< HEAD
=======
#include "Packet.hpp"

=======
>>>>>>> Stashed changes
<<<<<<< Updated upstream
};

=======
=======
<<<<<<< Updated upstream
=======
>>>>>>> c0efdf229e92f4978f156818e4f77faacca9e41e
>>>>>>> Stashed changes
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

template <typename T>
class Pool {
private:
    std::vector<std::unique_ptr<T>> m_pool;
<<<<<<< Updated upstream
=======
<<<<<<< HEAD
=======
>>>>>>> Stashed changes
    std::vector<std::unique_ptr<T>> m_pool_busy;

    void makeAvailable(std::unique_ptr<T>& t_object) {
        auto it = std::find_if(m_pool_busy.begin(), m_pool_busy.end(), 
            [&t_object](const std::unique_ptr<T>& uptr) { return uptr.get() == t_object.get(); });

        if (it != m_pool_busy.end()) {
            m_pool.push_back(std::move(*it));
            m_pool_busy.erase(it);
        }
    }
<<<<<<< Updated upstream
=======
>>>>>>> c0efdf229e92f4978f156818e4f77faacca9e41e
>>>>>>> Stashed changes

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

<<<<<<< Updated upstream
=======
<<<<<<< HEAD
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
=======
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
=======
>>>>>>> c0efdf229e92f4978f156818e4f77faacca9e41e
>>>>>>> Stashed changes
    }

    Pool() = default;
    ~Pool() = default;
};
>>>>>>> Stashed changes
<<<<<<< Updated upstream
=======
<<<<<<< HEAD
=======
>>>>>>> Stashed changes
>>>>>>> c0efdf229e92f4978f156818e4f77faacca9e41e
>>>>>>> Stashed changes
>>>>>>> Stashed changes
