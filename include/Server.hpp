#pragma once

#include <string>

namespace proxyServer {
    class Server {
    private:
        std::string m_ip_address;
    protected:
        bool initialiseServer();
        void serverLoop();
    public:
        Server();
        ~Server();
    };
}
