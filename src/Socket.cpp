#include "Socket.hpp"
#include "Logger.hpp"

#include <cstring>
#include <unistd.h>
#include <iostream>

proxyServer::Socket::Socket(unsigned short int t_port_number) 
    : port_number(t_port_number), socket_fd(-1), running(false) {}

proxyServer::Socket::~Socket() {
    if (running) {
        running = false;
        if (listen_thread.joinable()) {
            listen_thread.join();
        }
    }
    if (socket_fd != -1) {
        removeSocket();
    }
}

bool proxyServer::Socket::initialiseSocket() {
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1) {
        proxyServer::Logger::log("Failed to create socket: " + std::string(strerror(errno)), proxyServer::Logger::LogType::ERROR);
        return false;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_number);

    return bindSocket();
}

bool proxyServer::Socket::bindSocket() {
    if (bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        proxyServer::Logger::log("Bind failed: " + std::string(strerror(errno)), proxyServer::Logger::LogType::ERROR);
        return false;
    }
    proxyServer::Logger::log("Bind successful", proxyServer::Logger::LogType::SUCCESS);
    return true;
}

bool proxyServer::Socket::listenSocket() {
    if (listen(socket_fd, 3) < 0) {
        proxyServer::Logger::log("Failed to listen to the socket: " + std::string(strerror(errno)), proxyServer::Logger::LogType::ERROR);
        return false;
    }
    proxyServer::Logger::log("Listening on port " + std::to_string(port_number), proxyServer::Logger::LogType::SUCCESS);
    return true;
}

void proxyServer::Socket::configureListener() {
    if (!listenSocket()) {
        return;
    }
    
    running = true;
}

void proxyServer::Socket::startListener(proxyServer::Socket::ExecutionType t_execution_type) {
    switch (t_execution_type) {
        case proxyServer::Socket::ExecutionType::DEFERRED:
            listenLoop();
            break;
        case proxyServer::Socket::ExecutionType::ASYNC:
        {
            listen_thread = std::thread(&proxyServer::Socket::listenLoop, this);
            break;
        }
        default:
            proxyServer::Logger::log("An impossible type of Socket listen type was done, aborting operation", proxyServer::Logger::LogType::ERROR);
            break;
    }
}

bool proxyServer::Socket::removeSocket() {
    if (socket_fd != -1) {
        close(socket_fd);
        proxyServer::Logger::log("Socket " + std::to_string(socket_fd) + " closed.", proxyServer::Logger::LogType::SUCCESS);
        socket_fd = -1;
        return true;
    }
    return false;
}
