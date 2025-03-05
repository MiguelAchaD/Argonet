#pragma once

#include "Socket.hpp"
#include "Handler.hpp"

#include <string>

namespace proxyServer {
    class Server : public proxyServer::Socket {
    private:
        std::string m_ip_address;
        proxyServer::Handler handler;
    protected:
        bool initialiseServerSocket();
        void listenLoop() override;
    public:
        Server(unsigned short int t_port_number);
        ~Server();
        void startServer();
    };
}
