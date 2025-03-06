#pragma once

#include "Socket.hpp"

#include <string>

namespace proxyServer {
class Sender : proxyServer::Socket {
    protected:
        void sendToClient(std::string &t_response);
        void listenLoop() override;
    public:
        Sender(unsigned short int t_port_number);
        ~Sender();
};
}
