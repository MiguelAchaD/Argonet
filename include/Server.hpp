#pragma once

#include "Socket.hpp"
#include "Handler.hpp"
#include "Packet.hpp"
#include "Command.hpp"

#include <string>
#include <atomic>

namespace proxyServer {
<<<<<<< Updated upstream
    class Server : public proxyServer::Socket {
=======
<<<<<<< Updated upstream
class Server : public proxyServer::Socket {
=======
<<<<<<< Updated upstream
    class Server : public proxyServer::Socket {
>>>>>>> Stashed changes
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
    };
=======
<<<<<<< Updated upstream
};
=======
    };
=======
class Server : private Socket {
private:
    std::string m_ip_address;
    proxyServer::Handler handler;
    std::atomic<bool> m_is_running;

public:
    Server(unsigned short int t_port_number);
    ~Server();

    bool initialize(unsigned short int port_number);
    void start();
    void stop();
    bool isRunning() const;
    std::string getIPAddress() const;
};
>>>>>>> Stashed changes
>>>>>>> Stashed changes
>>>>>>> Stashed changes
}
