#pragma once
#include "Socket.hpp"
#include "Packet.hpp"
#include <string>

namespace proxyServer {
<<<<<<< Updated upstream
    class Sender : proxyServer::Socket {
=======
<<<<<<< Updated upstream
class Sender : proxyServer::Socket {
=======
<<<<<<< Updated upstream
    class Sender : proxyServer::Socket {
>>>>>>> Stashed changes
>>>>>>> Stashed changes
    protected:
        void sendToClient(std::string &t_response);
        void listenLoop() override;
        std::atomic<bool> running = true;
    public:
        Sender(unsigned short int t_port_number);
        ~Sender();
<<<<<<< Updated upstream
    };
=======
<<<<<<< Updated upstream
};
=======
    };
=======
class Sender : private Socket {
public:
    Sender(unsigned short int t_port_number);
    ~Sender();

    bool sendToClient(const petitionPacket packet);
    void closeClientConnection(int client_socket);
};
>>>>>>> Stashed changes
>>>>>>> Stashed changes
>>>>>>> Stashed changes
}
