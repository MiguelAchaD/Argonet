#pragma once
#include "Socket.hpp"
#include "Packet.hpp"
#include <string>

namespace proxyServer {
<<<<<<< Updated upstream
=======
<<<<<<< Updated upstream
    class Sender : proxyServer::Socket {
=======
<<<<<<< HEAD
<<<<<<< Updated upstream
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
<<<<<<< Updated upstream
};
=======
    };
=======
class Sender : private Socket {
=======
class Sender : private Socket {
private:
    void disconnect();
    void closeClientConnection(int client_socket);
>>>>>>> Stashed changes
public:
    Sender(unsigned short int t_port_number);
    ~Sender();

<<<<<<< Updated upstream
    bool sendToClient(const petitionPacket packet);
    void closeClientConnection(int client_socket);
};
>>>>>>> Stashed changes
>>>>>>> Stashed changes
>>>>>>> Stashed changes
=======
    void execute(const petitionPacket packet);
};
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
  class Sender : private Socket {
  public:
      Sender(unsigned short int t_port_number);
      ~Sender();

      bool sendToClient(const petitionPacket packet);
      void closeClientConnection(int client_socket);
  };
<<<<<<< Updated upstream
=======
>>>>>>> c0efdf229e92f4978f156818e4f77faacca9e41e
>>>>>>> Stashed changes
>>>>>>> Stashed changes
}
