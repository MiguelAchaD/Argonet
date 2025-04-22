<<<<<<< Updated upstream
=======
<<<<<<< Updated upstream
=======
>>>>>>> Stashed changes
#pragma once

#include "Api.hpp"
#include "Packet.hpp"

#include <vector>

namespace proxyServer {
class Resolver {
    private:
        std::vector<proxyServer::Api> m_apis;
<<<<<<< Updated upstream
    public:
        proxyServer::petitionPacket evaluateResults(proxyServer::petitionPacket t_packet);
=======

        unsigned short int ML_process();
    public:
        proxyServer::petitionPacket execute(proxyServer::petitionPacket t_packet);
>>>>>>> Stashed changes
        Resolver();
        ~Resolver();
};
}
<<<<<<< Updated upstream
=======
>>>>>>> Stashed changes
>>>>>>> Stashed changes
