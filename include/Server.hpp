#pragma once

#include "Socket.hpp"
#include "Handler.hpp"
#include "Packet.hpp"
#include "Command.hpp"
#include <atomic>
#include <sqlite3.h>

#include <string>
#include <atomic>

namespace proxyServer {
class Server : private Socket {
  private:
      std::string m_ip_address;
      proxyServer::Handler handler;
      std::atomic<bool> m_is_running;
      sqlite3 *db;

  public:
      Server(unsigned short int t_port_number);
      ~Server();

      bool initialize(unsigned short int port_number);
      void start();
      void stop();
      bool isRunning() const;
      std::string getIPAddress() const;
  };
}
