#pragma once

#include "Socket.hpp"
#include "Packet.hpp"

#include <netdb.h>
#include <string>

namespace proxyServer {
  class Forwarder : public Socket {
  private:
      bool connect(const FastString& host, int port = 80);
      std::string sendRequest(const proxyServer::petitionPacket& packet);
      std::string receiveResponse(size_t buffer_size = 4096);
      void disconnect();
      std::string decompressGzip(const std::string& compressed_data);
      std::string removeHeader(const std::string& headers, const std::string& header_name);

  public:
      Forwarder(unsigned short int t_port_number);
      ~Forwarder();

      proxyServer::petitionPacket execute(proxyServer::petitionPacket t_packet);
  };
}
