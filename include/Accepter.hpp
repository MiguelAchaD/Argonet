#pragma once

#include "Packet.hpp"

#include <string>

namespace proxyServer {
class Accepter {
    private:
        proxyServer::petitionPacket parseContents(std::string t_result);
        std::string readContents(int t_client_socket);
    public:
        proxyServer::petitionPacket execute(int t_client_socket);
        Accepter();
        ~Accepter();
};
}
