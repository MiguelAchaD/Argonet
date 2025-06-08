#pragma once

#include <string>
#include <map>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <nlohmann/json.hpp>
#include "Command.hpp"

namespace proxyServer {
namespace web {

class WebServer {
public:
    WebServer(unsigned short port);
    ~WebServer();

    void start();
    void stop();
    bool isRunning() const;

private:
    void handleRequest(int clientSocket);
    void setupRoutes();
    std::string generateToken(const std::string& username);
    bool validateToken(const std::string& token);
    std::string readStaticFile(const std::string& path);
    std::string getContentType(const std::string& path);
    
    // Route handlers
    std::string handleLogin(const nlohmann::json& request);
    std::string handleRegister(const nlohmann::json& request);
    std::string handleGetConfig(const nlohmann::json& request);
    std::string handleUpdateConfig(const nlohmann::json& request);
    std::string handleGetStatus(const nlohmann::json& request);

    unsigned short m_port;
    int m_serverSocket;
    bool m_running;
    std::thread m_serverThread;
    std::mutex m_mutex;
    
    // In-memory storage for sessions (in a real app, use a proper session store)
    std::map<std::string, std::string> m_sessions;
    
    // Route handlers map
    std::map<std::string, std::function<std::string(const nlohmann::json&)>> m_routes;
};

} // namespace web
} // namespace proxyServer 