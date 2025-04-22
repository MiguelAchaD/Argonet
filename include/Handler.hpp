#pragma once

#include "Sender.hpp"
#include "Forwarder.hpp"
#include "Accepter.hpp"
#include "Pool.hpp"
#include "Resolver.hpp"
#include "Pool.hpp"
#include "Packet.hpp"

#include <future>
#include <vector>
#include <string>

namespace proxyServer {
class Handler {
    private:
        std::vector<unsigned int> m_port_pool;
        std::vector<unsigned int> m_port_busy_pool;
        
        Pool<proxyServer::Accepter> m_accepter_pool;
        Pool<proxyServer::Resolver> m_resolver_pool;
        Pool<proxyServer::Forwarder> m_forwarder_pool;
        Pool<proxyServer::Sender> m_sender_pool;
  
        std::vector<std::future<proxyServer::petitionPacket>> accepterFutures;
        std::vector<std::future<proxyServer::petitionPacket>> resolverFutures;
        std::vector<std::future<proxyServer::petitionPacket>> forwarderFutures;
 
        template <typename FutureContainer, typename ProcessFunction>
        void processFutures(FutureContainer& futures, ProcessFunction processFunction);
  
    public:
        void accepterInvoke(int t_client_socket);
        void resolverInvoke(proxyServer::petitionPacket t_packet);
        void forwarderInvoke(proxyServer::petitionPacket t_packet);
        void senderInvoke(proxyServer::petitionPacket t_packet);

        unsigned short int pickPort();

        void checkFutures();

        Handler();
        ~Handler();
};
}
