#pragma once

#include "Socket.hpp"
#include "Handler.hpp"
#include "Packet.hpp"
#include "Command.hpp"
#include <atomic>
#include <sqlite3.h>
#include <chrono>

#include <string>
#include <atomic>
#include <thread>
#include <chrono>

namespace proxyServer {
class Server : private Socket {
  private:
      std::string m_ip_address;
      proxyServer::Handler handler;
      std::atomic<bool> m_is_running;
      sqlite3 *db;
      static Server* instance;
      std::chrono::steady_clock::time_point m_start_time;
      std::atomic<size_t> m_active_connections{0};

  public:
      Server(unsigned short int t_port_number);
      ~Server();

      static Server& getInstance(unsigned short int port = 8080) {
          if (instance == nullptr) {
              instance = new Server(port);
          }
          return *instance;
      }

      bool initialize(unsigned short int port_number);
      void start();
      void stop();
      bool isRunning() const;
      std::string getIPAddress() const;
      void updateConfiguration();
      void wait() {
          while (m_is_running) {
              std::this_thread::sleep_for(std::chrono::seconds(1));
          }
      }

      size_t getWorkerCount(const std::string& pool_type) const {
          return handler.getWorkerCount(pool_type);
      }

      std::string getUptime() const {
          auto now = std::chrono::steady_clock::now();
          auto duration = std::chrono::duration_cast<std::chrono::seconds>(now - m_start_time);
          return std::to_string(duration.count());
      }

      size_t getActiveConnections() const {
          return m_active_connections;
      }

      void incrementActiveConnections() {
          m_active_connections++;
      }

      void decrementActiveConnections() {
          if (m_active_connections > 0) {
              m_active_connections--;
          }
      }
};
}
