#pragma once

#include "Socket.hpp"
#include "Packet.hpp"

#include <netdb.h>
#include <string>

namespace proxyServer {
<<<<<<< Updated upstream
    class Forwarder : proxyServer::Socket {
=======
<<<<<<< Updated upstream
class Forwarder : proxyServer::Socket {
=======
<<<<<<< Updated upstream
    class Forwarder : proxyServer::Socket {
>>>>>>> Stashed changes
>>>>>>> Stashed changes
    private:
        std::string resolveDns();
        std::string fechContents();
    protected:
        std::string fetch();
        void listenLoop() override;
        std::atomic<bool> running = true;
    public:
        Forwarder(unsigned short int t_port_number);
        ~Forwarder();
<<<<<<< Updated upstream
    };
=======
<<<<<<< Updated upstream
        
};
=======
=======
class Forwarder : private Socket {
private:
    bool connect(const std::string& host, int port = 80);
    std::string sendRequest(const proxyServer::petitionPacket& packet);
    std::string receiveResponse(size_t buffer_size = 4096);
    void disconnect();

public:
    Forwarder(unsigned short int t_port_number);
    ~Forwarder();

    proxyServer::petitionPacket fetch(proxyServer::petitionPacket t_packet);

>>>>>>> Stashed changes
    };
>>>>>>> Stashed changes
>>>>>>> Stashed changes
}
