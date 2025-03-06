#pragma once

#include <vector>

#include "Sender.hpp"
#include "Forwarder.hpp"
#include "Accepter.hpp"
#include "Pool.hpp"

namespace proxyServer {
class Handler {
    private:
        std::vector<unsigned int> m_port_pool;
        std::vector<unsigned int> m_port_busy_pool;
        Pool<proxyServer::Accepter> m_accepter_pool;
        Pool<proxyServer::Forwarder> m_forwarder_pool;
        Pool<proxyServer::Sender> m_sender_pool;
 
        unsigned short int pickPort();
        
    public:
        Handler();
        ~Handler();
};
}
