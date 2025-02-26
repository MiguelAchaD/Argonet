#pragma once

#include <vector>

#include "Sockets/Sender.hpp"
#include "Sockets/Forwarder.hpp"
#include "Sockets/Accepter.hpp"

namespace proxyServer {
    class Handler {
    protected:
        enum class Type {
            ACCEPTER,
            FORWARDER,
            SENDER
        };

    private:
        std::vector<Accepter> m_accepter_pool;
        std::vector<Forwarder> m_forwarder_pool;
        std::vector<Sender> m_sender_pool;
        bool addToPool(Type t_type);
        bool removeFromPool(Type t_type);
        
    public:
        Handler();
        ~Handler();
    };
}
