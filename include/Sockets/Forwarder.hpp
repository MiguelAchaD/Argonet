#pragma once

#include "Socket.hpp"
#include "Packet.hpp"

#include <netdb.h>
#include <string>

namespace proxyServer {
class Forwarder : private Socket {
  private:
      bool connect(const std::string& host, int port = 80);
      std::string sendRequest(const proxyServer::petitionPacket& packet);
      std::string receiveResponse(size_t buffer_size = 4096);
      void disconnect();

  public:
      Forwarder(unsigned short int t_port_number);
      ~Forwarder();

      proxyServer::petitionPacket execute(proxyServer::petitionPacket t_packet);
  };
}
