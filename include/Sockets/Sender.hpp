#pragma once
#include "Socket.hpp"
#include "Packet.hpp"
#include <string>

namespace proxyServer {
  class Sender : private Socket {
  public:
      Sender(unsigned short int t_port_number);
      ~Sender();

      bool sendToClient(const petitionPacket packet);
      void closeClientConnection(int client_socket);
  };
}
