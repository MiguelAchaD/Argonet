#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>

namespace proxyServer {
namespace web {

class WebServer {
public:
    explicit WebServer(unsigned short port);
    ~WebServer();

    void start();
    void stop();
    bool isRunning() const;

private:
    void setupRoutes();
    void handleRequest(int clientSocket);
    std::string generateToken(const std::string& username);
    bool validateToken(const std::string& token);
    std::string readStaticFile(const std::string& path);
    std::string getContentType(const std::string& path);

    // API route handlers
    std::string handleLogin(const nlohmann::json& req);
    std::string handleRegister(const nlohmann::json& req);
    std::string handleGetConfig(const nlohmann::json& req);
    std::string handleUpdateConfig(const nlohmann::json& req);
    std::string handleGetStatus(const nlohmann::json& req);

    unsigned short m_port;
    int m_serverSocket;
    std::atomic<bool> m_running;
    std::thread m_serverThread;
    std::mutex m_mutex;
    std::unordered_map<std::string, std::string> m_sessions;
    std::unordered_map<std::string, std::function<std::string(const nlohmann::json&)>> m_routes;
};

} // namespace web
} // namespace proxyServer 