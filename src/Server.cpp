#include "Server.hpp"
#include "Logger.hpp"
#include "Blacklist.hpp"
#include "Command.hpp"
#include "APIs/APIKeyManager.hpp"
#include <stdexcept>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

namespace proxyServer {

Server* Server::instance = nullptr;

Server::Server(unsigned short int port) : Socket(port), m_start_time(std::chrono::steady_clock::now()) {
    try {        
        // Initialize API keys
        APIKeyManager::getInstance().loadFromConfig("configuration.json");
        
        // Set server reference in handler
        handler.setServer(this);
        
    } catch (const std::exception& e) {
        Logger::log("Server initialization error: " + std::string(e.what()), 
                   Logger::LogType::ERROR);
        throw;
    }
}

Server::~Server() {
    stop();
}

void Server::updateConfiguration() {
    try {
        // Read and validate the new configuration
        nlohmann::json config = Command::readConfigurationFile();
        if (!Command::validateConfigurationFile(config)) {
            throw std::runtime_error("Invalid configuration file");
        }

        // Update the configuration file
        if (!Command::updateConfigurationFile(config)) {
            throw std::runtime_error("Failed to update configuration file");
        }

        // Update blacklist
        Blacklist::getInstance().loadFromConfig("configuration.json");

        // Update API keys
        APIKeyManager::getInstance().loadFromConfig("configuration.json");

        // Update pool configurations
        handler.updatePoolConfiguration(config);

        Logger::log("Configuration updated successfully", Logger::LogType::SUCCESS);
    } catch (const std::exception& e) {
        Logger::log("Failed to update configuration: " + std::string(e.what()), 
                   Logger::LogType::ERROR);
        throw;
    }
}

void Server::start() {
    if (running) {
        Logger::log("Server is already running", Logger::LogType::WARNING);
        return;
    }

    try {
        // Create socket
        if (!createSocket()) {
            throw std::runtime_error("Failed to create socket");
        }

        // Set socket options
        int opt = 1;
        if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            throw std::runtime_error("Failed to set socket options");
        }

        // Bind socket
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port_number);

        if (bind(socket_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
            throw std::runtime_error("Failed to bind socket");
        }

        // Listen for connections
        if (listen(socket_fd, SOMAXCONN) < 0) {
            throw std::runtime_error("Failed to listen on socket");
        }

        running = true;
        m_start_time = std::chrono::steady_clock::now();
        Logger::log("Server started " + Command::getActiveInterfaceIP() + ":" + std::to_string(port_number), Logger::LogType::SUCCESS);

        // Start accepting connections
        while (running) {
            try {
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int client_socket = accept(socket_fd, (struct sockaddr*)&client_addr, &client_len);
                
                if (client_socket < 0) {
                    if (running) {
                        Logger::log("Error accepting connection", Logger::LogType::ERROR);
                    }
                    continue;
                }

                incrementActiveConnections();

                // Create new packet for the connection
                petitionPacket packet;
                packet.client_socket = client_socket;

                // Add to work queue
                handler.handlePacket(std::move(packet));
            } catch (const std::exception& e) {
                if (running) {
                    Logger::log("Error in connection handling: " + std::string(e.what()), Logger::LogType::ERROR);
                }
            }
        }
    } catch (const std::exception& e) {
        Logger::log("Server error: " + std::string(e.what()), Logger::LogType::ERROR);
        stop();
        throw;
    }
}

void Server::stop() {
    if (!running) {
        return;
    }

    running = false;
    closeSocket();
    Logger::log("Server stopped", Logger::LogType::LOG);
}

bool Server::isRunning() const {
    return running;
}

} // namespace proxyServer
