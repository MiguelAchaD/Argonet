#pragma once

#include "Sender.hpp"
#include "Forwarder.hpp"
#include "Accepter.hpp"
<<<<<<< Updated upstream
=======
<<<<<<< Updated upstream
#include "Pool.hpp"
=======
<<<<<<< Updated upstream
=======
#include "Resolver.hpp"
#include "Pool.hpp"
#include "Packet.hpp"

#include <future>
#include <vector>
#include <string>
>>>>>>> Stashed changes
>>>>>>> Stashed changes
>>>>>>> Stashed changes

namespace proxyServer {
    class Handler {
    protected:
        enum class Type {
            ACCEPTER,
            FORWARDER,
            SENDER
        };
        

    private:
        std::vector<unsigned int> m_port_pool;
        std::vector<unsigned int> m_port_busy_pool;
<<<<<<< Updated upstream
        std::vector<std::unique_ptr<Accepter>> m_accepter_pool;
        std::vector<std::unique_ptr<Forwarder>> m_forwarder_pool;
        std::vector<std::unique_ptr<Sender>> m_sender_pool;
        
        bool addToPool(Type t_type);
        bool removeFromPool(Type t_type);
        unsigned short int pickPort();
=======
<<<<<<< Updated upstream
        Pool<proxyServer::Accepter> m_accepter_pool;
        Pool<proxyServer::Forwarder> m_forwarder_pool;
        Pool<proxyServer::Sender> m_sender_pool;
 
=======
<<<<<<< Updated upstream
        std::vector<std::unique_ptr<Accepter>> m_accepter_pool;
        std::vector<std::unique_ptr<Forwarder>> m_forwarder_pool;
        std::vector<std::unique_ptr<Sender>> m_sender_pool;
>>>>>>> Stashed changes
        
        bool addToPool(Type t_type);
        bool removeFromPool(Type t_type);
=======

        std::vector<std::future<proxyServer::petitionPacket>> accepterFutures;
        std::vector<std::future<proxyServer::petitionPacket>> resolverFutures;
        std::vector<std::future<proxyServer::petitionPacket>> forwarderFutures;

        Pool<proxyServer::Accepter> m_accepter_pool;
        Pool<proxyServer::Resolver> m_resolver_pool;
        Pool<proxyServer::Forwarder> m_forwarder_pool;
        Pool<proxyServer::Sender> m_sender_pool;
 
        template <typename FutureContainer, typename ProcessFunction>
        void processFutures(FutureContainer& futures, ProcessFunction processFunction);
>>>>>>> Stashed changes
>>>>>>> Stashed changes
        unsigned short int pickPort();
    public:
        void accepterInvoke(int t_client_socket);
        void resolverInvoke(proxyServer::petitionPacket t_packet);
        void forwarderInvoke(proxyServer::petitionPacket t_packet);
        void senderInvoke(proxyServer::petitionPacket t_packet);

        void checkFutures();

        Handler();
        ~Handler();
    };
}
