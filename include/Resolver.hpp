#pragma once

#include "Api.hpp"
#include "Packet.hpp"

#include <vector>

namespace proxyServer {
class Resolver {
    private:
        std::vector<proxyServer::Api> m_apis;
    public:
        proxyServer::petitionPacket evaluateResults(proxyServer::petitionPacket t_packet);
        Resolver();
        ~Resolver();
};
}
