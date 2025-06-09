#include "Server.hpp"
#include "Logger.hpp"
#include "Blacklist.hpp"
#include <stdexcept>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

namespace proxyServer {

Server::Server(unsigned short int port) : Socket(port) {
    try {
        // Initialize blacklist
        Blacklist::getInstance().loadFromConfig("configuration.json");
        
    } catch (const std::exception& e) {
        Logger::log("Server initialization error: " + std::string(e.what()), 
                   Logger::LogType::ERROR);
        throw;
    }
}

Server::~Server() {
    stop();
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
        Logger::log("Server started successfully");

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
