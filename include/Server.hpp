#pragma once

#include "Socket.hpp"
#include "Handler.hpp"
#include "Packet.hpp"
#include "Command.hpp"

#include <string>
<<<<<<< Updated upstream
#include <atomic>

namespace proxyServer {
=======
<<<<<<< Updated upstream

namespace proxyServer {
    class Server : public proxyServer::Socket {
=======
#include <atomic>
#include <sqlite3.h>

namespace proxyServer {
<<<<<<< HEAD
<<<<<<< Updated upstream
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
=======
  class Server : private Socket {
  private:
    std::string m_ip_address;
    proxyServer::Handler handler;
    std::atomic<bool> m_is_running;
    sqlite3* db;

  public:
>>>>>>> Stashed changes
    Server(unsigned short int t_port_number);
    ~Server();

    bool initialize(unsigned short int port_number);
    void start();
    void stop();
    bool isRunning() const;
    std::string getIPAddress() const;
<<<<<<< Updated upstream
};
>>>>>>> Stashed changes
>>>>>>> Stashed changes
>>>>>>> Stashed changes
=======
  };
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
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
<<<<<<< Updated upstream
=======
>>>>>>> c0efdf229e92f4978f156818e4f77faacca9e41e
>>>>>>> Stashed changes
>>>>>>> Stashed changes
}
