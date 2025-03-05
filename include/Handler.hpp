#pragma once

#include <vector>

#include "Sender.hpp"
#include "Forwarder.hpp"
#include "Accepter.hpp"

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
        std::vector<std::unique_ptr<Accepter>> m_accepter_pool;
        std::vector<std::unique_ptr<Forwarder>> m_forwarder_pool;
        std::vector<std::unique_ptr<Sender>> m_sender_pool;
        
        bool addToPool(Type t_type);
        bool removeFromPool(Type t_type);
        unsigned short int pickPort();
        
    public:
        Handler();
        ~Handler();
    };
}
