#pragma once

#include "Packet.hpp"

#include <string>

namespace proxyServer {
class Accepter {
    private:
        proxyServer::petitionPacket parseContents(std::string t_result);
        std::string readContents(int t_client_socket);
    public:
<<<<<<< Updated upstream
        proxyServer::petitionPacket examineContents(int t_client_socket);
=======
<<<<<<< Updated upstream
=======
        proxyServer::petitionPacket execute(int t_client_socket);
>>>>>>> Stashed changes
>>>>>>> Stashed changes
        Accepter();
        ~Accepter();
};
}
