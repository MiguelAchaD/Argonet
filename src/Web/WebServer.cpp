#include "WebServer.hpp"
#include "Logger.hpp"
#include "Command.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <regex>

namespace proxyServer {
namespace web {

WebServer::WebServer(unsigned short port) 
    : m_port(port), m_serverSocket(-1), m_running(false) {
    setupRoutes();
}

WebServer::~WebServer() {
    stop();
}

void WebServer::start() {
    m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (m_serverSocket < 0) {
        Logger::log("Failed to create socket", Logger::LogType::ERROR);
        return;
    }

    int opt = 1;
    if (setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        Logger::log("Failed to set socket options", Logger::LogType::ERROR);
        return;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(m_port);

    if (bind(m_serverSocket, (struct sockaddr*)&address, sizeof(address)) < 0) {
        Logger::log("Failed to bind socket", Logger::LogType::ERROR);
        return;
    }

    if (listen(m_serverSocket, 3) < 0) {
        Logger::log("Failed to listen on socket", Logger::LogType::ERROR);
        return;
    }

    Logger::log("Web server started http://" + Command::getActiveInterfaceIP() + ":" + std::to_string(m_port), Logger::LogType::SUCCESS);
    m_running = true;
    m_serverThread = std::thread([this]() {
        while (m_running) {
            int clientSocket = accept(m_serverSocket, nullptr, nullptr);
            if (clientSocket < 0) {
                Logger::log("Failed to accept connection", Logger::LogType::ERROR);
                continue;
            }
            handleRequest(clientSocket);
            close(clientSocket);
        }
    });
}

void WebServer::stop() {
    m_running = false;
    if (m_serverThread.joinable()) {
        m_serverThread.join();
    }
    if (m_serverSocket >= 0) {
        close(m_serverSocket);
        m_serverSocket = -1;
    }
}

bool WebServer::isRunning() const {
    return m_running;
}

void WebServer::setupRoutes() {
    m_routes["/api/login"] = [this](const nlohmann::json& req) { return handleLogin(req); };
    m_routes["/api/register"] = [this](const nlohmann::json& req) { return handleRegister(req); };
    m_routes["/api/config"] = [this](const nlohmann::json& req) { return handleGetConfig(req); };
    m_routes["/api/config/update"] = [this](const nlohmann::json& req) { return handleUpdateConfig(req); };
    m_routes["/api/config/reset"] = [this](const nlohmann::json& req) { return handleResetConfig(req); };
    m_routes["/api/status"] = [this](const nlohmann::json& req) { return handleGetStatus(req); };
}

std::string WebServer::generateToken(const std::string& username) {
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    const char* hex = "0123456789abcdef";
    
    std::string token;
    for (int i = 0; i < 32; ++i) {
        token += hex[dis(gen)];
    }
    
    std::lock_guard<std::mutex> lock(m_mutex);
    m_sessions[token] = username;
    return token;
}

bool WebServer::validateToken(const std::string& token) {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_sessions.find(token) != m_sessions.end();
}

std::string WebServer::readStaticFile(const std::string& path) {
    std::string fullPath = "assets/static" + path;
    std::ifstream file(fullPath);
    if (!file.is_open()) {
        Logger::log("Failed to open file: " + fullPath, Logger::LogType::ERROR);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string WebServer::getContentType(const std::string& path) {
    if (path.length() >= 5 && path.substr(path.length() - 5) == ".html") return "text/html";
    if (path.length() >= 4 && path.substr(path.length() - 4) == ".css") return "text/css";
    if (path.length() >= 3 && path.substr(path.length() - 3) == ".js") return "application/javascript";
    if (path.length() >= 5 && path.substr(path.length() - 5) == ".json") return "application/json";
    if (path.length() >= 4 && path.substr(path.length() - 4) == ".svg") return "image/svg+xml";
    return "text/plain";
}

void WebServer::handleRequest(int clientSocket) {
    char buffer[4096] = {0};
    int bytesRead = read(clientSocket, buffer, sizeof(buffer) - 1);
    if (bytesRead <= 0) {
        return;
    }

    std::string request(buffer);
    std::istringstream requestStream(request);
    std::string method, path, version;
    requestStream >> method >> path >> version;

    // Handle API requests first
    if (path.length() >= 5 && path.substr(0, 5) == "/api/") {
        try {
            nlohmann::json requestJson;
            requestJson["path"] = path;
            
            // Parse request body for POST requests
            if (method == "POST") {
                std::string body;
                size_t contentLength = 0;
                
                // Find Content-Length header
                std::string line;
                while (std::getline(requestStream, line) && !line.empty()) {
                    if (line.find("Content-Length:") != std::string::npos) {
                        contentLength = std::stoul(line.substr(16));
                    }
                }
                
                // Read request body
                if (contentLength > 0) {
                    body.resize(contentLength);
                    size_t bodyStart = request.find("\r\n\r\n") + 4;
                    if (bodyStart < request.length()) {
                        body = request.substr(bodyStart);
                        try {
                            requestJson = nlohmann::json::parse(body);
                        } catch (const std::exception& e) {
                            Logger::log("Failed to parse JSON body: " + std::string(e.what()), Logger::LogType::ERROR);
                        }
                    }
                }
            }
            
            auto it = m_routes.find(path);
            if (it != m_routes.end()) {
                std::string response = it->second(requestJson);
                std::string httpResponse = "HTTP/1.1 200 OK\r\n";
                httpResponse += "Content-Type: application/json\r\n";
                httpResponse += "Access-Control-Allow-Origin: *\r\n";
                httpResponse += "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
                httpResponse += "Access-Control-Allow-Headers: Content-Type\r\n";
                httpResponse += "Content-Length: " + std::to_string(response.length()) + "\r\n";
                httpResponse += "\r\n";
                httpResponse += response;
                send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);
            } else {
                std::string error = "{\"status\":\"error\",\"message\":\"Route not found\"}";
                std::string httpResponse = "HTTP/1.1 404 Not Found\r\n";
                httpResponse += "Content-Type: application/json\r\n";
                httpResponse += "Access-Control-Allow-Origin: *\r\n";
                httpResponse += "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
                httpResponse += "Access-Control-Allow-Headers: Content-Type\r\n";
                httpResponse += "Content-Length: " + std::to_string(error.length()) + "\r\n";
                httpResponse += "\r\n";
                httpResponse += error;
                send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);
            }
        } catch (const std::exception& e) {
            std::string error = "{\"status\":\"error\",\"message\":\"" + std::string(e.what()) + "\"}";
            std::string httpResponse = "HTTP/1.1 500 Internal Server Error\r\n";
            httpResponse += "Content-Type: application/json\r\n";
            httpResponse += "Access-Control-Allow-Origin: *\r\n";
            httpResponse += "Access-Control-Allow-Methods: GET, POST, OPTIONS\r\n";
            httpResponse += "Access-Control-Allow-Headers: Content-Type\r\n";
            httpResponse += "Content-Length: " + std::to_string(error.length()) + "\r\n";
            httpResponse += "\r\n";
            httpResponse += error;
            send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);
        }
        return;
    }

    // Handle static file requests
    if (method == "GET") {
        if (path == "/") {
            path = "/index.html";
        }

        // Handle static files
        std::string content = readStaticFile(path);
        if (!content.empty()) {
            std::string contentType = getContentType(path);
            std::string httpResponse = "HTTP/1.1 200 OK\r\n";
            httpResponse += "Content-Type: " + contentType + "\r\n";
            httpResponse += "Access-Control-Allow-Origin: *\r\n";
            httpResponse += "Content-Length: " + std::to_string(content.length()) + "\r\n";
            httpResponse += "\r\n";
            httpResponse += content;
            send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);
        } else {
            std::string error = "404 Not Found";
            std::string httpResponse = "HTTP/1.1 404 Not Found\r\n";
            httpResponse += "Content-Type: text/plain\r\n";
            httpResponse += "Content-Length: " + std::to_string(error.length()) + "\r\n";
            httpResponse += "\r\n";
            httpResponse += error;
            send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);
        }
    } else {
        std::string error = "405 Method Not Allowed";
        std::string httpResponse = "HTTP/1.1 405 Method Not Allowed\r\n";
        httpResponse += "Content-Type: text/plain\r\n";
        httpResponse += "Content-Length: " + std::to_string(error.length()) + "\r\n";
        httpResponse += "\r\n";
        httpResponse += error;
        send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);
    }
}

std::string WebServer::handleLogin(const nlohmann::json& request) {
    // TODO: Implement proper user authentication
    std::string username = request["username"].get<std::string>();
    std::string password = request["password"].get<std::string>();
    
    // For now, accept any login
    std::string token = generateToken(username);
    
    nlohmann::json response = {
        {"status", "success"},
        {"token", token}
    };
    return response.dump();
}

std::string WebServer::handleRegister(const nlohmann::json& request) {
    // TODO: Implement user registration
    std::string username = request["username"].get<std::string>();
    std::string password = request["password"].get<std::string>();
    
    nlohmann::json response = {
        {"status", "success"},
        {"message", "User registered successfully"}
    };
    return response.dump();
}

std::string WebServer::handleGetConfig(const nlohmann::json& request) {
    if (!validateToken(request["token"].get<std::string>())) {
        nlohmann::json response = {
            {"status", "error"},
            {"message", "Invalid token"}
        };
        return response.dump();
    }
    
    nlohmann::json config = Command::readConfigurationFile();
    return config.dump();
}

std::string WebServer::handleUpdateConfig(const nlohmann::json& request) {    
    if (!validateToken(request["token"].get<std::string>())) {
        Logger::log("Invalid token in update config request", Logger::LogType::ERROR);
        nlohmann::json response = {
            {"status", "error"},
            {"message", "Invalid token"}
        };
        return response.dump();
    }
    
    try {
        // Get the new configuration from the request
        nlohmann::json new_config = request["config"];
        Logger::log("New configuration: " + new_config.dump(), Logger::LogType::LOG);
        
        // Validate VirusTotal configuration
        if (new_config.contains("virustotal")) {
            const auto& vt_config = new_config["virustotal"];
            if (vt_config.contains("api_keys") && vt_config["api_keys"].is_array()) {
                // Mask API keys in logs
                auto masked_config = new_config;
                masked_config["virustotal"]["api_keys"] = std::vector<std::string>(vt_config["api_keys"].size(), "****");
                Logger::log("Updating VirusTotal configuration: " + masked_config.dump(), Logger::LogType::LOG);
            }
        }
        
        // Update the configuration file
        if (!Command::updateConfigurationFile(new_config)) {
            Logger::log("Failed to update configuration file", Logger::LogType::ERROR);
            nlohmann::json response = {
                {"status", "error"},
                {"message", "Failed to update configuration"}
            };
            return response.dump();
        }

        // Update the server configuration
        if (m_server) {
            m_server->updateConfiguration();
        } else {
            Logger::log("Server reference not set, configuration not applied", Logger::LogType::WARNING);
        }
        
        // Return success response with the new configuration (with masked API keys)
        if (new_config.contains("virustotal") && new_config["virustotal"].contains("api_keys")) {
            new_config["virustotal"]["api_keys"] = std::vector<std::string>(
                new_config["virustotal"]["api_keys"].size(), 
                "****"
            );
        }
        
        nlohmann::json response = {
            {"status", "success"},
            {"message", "Configuration updated successfully"},
            {"config", new_config}
        };
        return response.dump();
    } catch (const std::exception& e) {
        Logger::log("Exception in update config: " + std::string(e.what()), Logger::LogType::ERROR);
        nlohmann::json response = {
            {"status", "error"},
            {"message", "Failed to update configuration: " + std::string(e.what())}
        };
        return response.dump();
    }
}

std::string WebServer::handleResetConfig(const nlohmann::json& request) {
    Logger::log("Received reset config request", Logger::LogType::LOG);
    
    if (!validateToken(request["token"].get<std::string>())) {
        Logger::log("Invalid token in reset config request", Logger::LogType::ERROR);
        nlohmann::json response = {
            {"status", "error"},
            {"message", "Invalid token"}
        };
        return response.dump();
    }
    
    try {
        // Read the default configuration
        std::ifstream defaultFile("configuration.default.json");
        if (!defaultFile.is_open()) {
            Logger::log("Failed to open default configuration file", Logger::LogType::ERROR);
            nlohmann::json response = {
                {"status", "error"},
                {"message", "Default configuration file not found"}
            };
            return response.dump();
        }

        nlohmann::json defaultConfig;
        defaultFile >> defaultConfig;
        defaultFile.close();

        // Update the configuration file with default values
        if (!Command::updateConfigurationFile(defaultConfig)) {
            Logger::log("Failed to reset configuration file", Logger::LogType::ERROR);
            nlohmann::json response = {
                {"status", "error"},
                {"message", "Failed to reset configuration"}
            };
            return response.dump();
        }

        // Update the server configuration
        if (m_server) {
            m_server->updateConfiguration();
        } else {
            Logger::log("Server reference not set, configuration not applied", Logger::LogType::WARNING);
        }
        
        // Return success response with the new configuration
        nlohmann::json response = {
            {"status", "success"},
            {"message", "Configuration reset successfully"},
            {"config", defaultConfig}
        };
        Logger::log("Configuration reset successfully", Logger::LogType::SUCCESS);
        return response.dump();
    } catch (const std::exception& e) {
        Logger::log("Exception in reset config: " + std::string(e.what()), Logger::LogType::ERROR);
        nlohmann::json response = {
            {"status", "error"},
            {"message", "Failed to reset configuration: " + std::string(e.what())}
        };
        return response.dump();
    }
}

std::string WebServer::handleGetStatus(const nlohmann::json& request) {
    if (!validateToken(request["token"].get<std::string>())) {
        nlohmann::json response = {
            {"status", "error"},
            {"message", "Invalid token"}
        };
        return response.dump();
    }
    
    try {
        if (!m_server) {
            throw std::runtime_error("Server reference not set");
        }

        nlohmann::json status = {
            {"status", m_server->isRunning() ? "running" : "stopped"},
            {"uptime", m_server->getUptime()},
            {"active_connections", m_server->getActiveConnections()},
            {"workers", {
                {"accepter", m_server->getWorkerCount("accepter")},
                {"resolver", m_server->getWorkerCount("resolver")},
                {"forwarder", m_server->getWorkerCount("forwarder")},
                {"sender", m_server->getWorkerCount("sender")}
            }}
        };
        return status.dump();
    } catch (const std::exception& e) {
        Logger::log("Error getting status: " + std::string(e.what()), Logger::LogType::ERROR);
        nlohmann::json response = {
            {"status", "error"},
            {"message", "Failed to get status: " + std::string(e.what())}
        };
        return response.dump();
    }
}

} // namespace web
} // namespace proxyServer 