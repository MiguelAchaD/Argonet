#pragma once

#include "Api.hpp"
#include "Packet.hpp"

#include <vector>

namespace proxyServer {
class Resolver {
    private:
        std::vector<proxyServer::Api> m_apis;
    public:
        proxyServer::petitionPacket execute(proxyServer::petitionPacket t_packet);
        Resolver();
        ~Resolver();
};
}
