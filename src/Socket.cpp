#include "Socket.hpp"
#include "Logger.hpp"
#include <unistd.h>
#include <cstring>

proxyServer::Socket::Socket(unsigned short int t_port_number) 
    : socket_fd(-1), port_number(t_port_number), running(false) {}

proxyServer::Socket::~Socket() {
<<<<<<< Updated upstream
=======
<<<<<<< Updated upstream
    if (running) {
        running = false;
        if (listen_thread.joinable()) {
            listen_thread.join();
        }
    }
=======
>>>>>>> Stashed changes
    closeSocket();
}

bool proxyServer::Socket::createSocket(int socket_type) {
    // Verificar si ya existe un socket abierto
<<<<<<< Updated upstream
=======
>>>>>>> Stashed changes
>>>>>>> Stashed changes
    if (socket_fd != -1) {
        closeSocket();
    }

<<<<<<< Updated upstream
    // Crear nuevo socket
    socket_fd = socket(AF_INET, socket_type, 0);
=======
<<<<<<< Updated upstream
bool proxyServer::Socket::initialiseSocket() {
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
=======
    // Crear nuevo socket
    socket_fd = socket(AF_INET, socket_type, 0);
>>>>>>> Stashed changes
>>>>>>> Stashed changes
    if (socket_fd == -1) {
        Logger::log("Failed to create socket: " + std::string(strerror(errno)), 
                    Logger::LogType::ERROR);
        return false;
    }

    return true;
}

bool proxyServer::Socket::setSocketTimeout(int seconds) {
    if (socket_fd == -1) {
        Logger::log("Cannot set timeout on invalid socket", Logger::LogType::ERROR);
        return false;
    }
<<<<<<< Updated upstream
=======
<<<<<<< Updated upstream
    proxyServer::Logger::log("Listening on port " + std::to_string(port_number), proxyServer::Logger::LogType::SUCCESS);
=======
>>>>>>> Stashed changes

    struct timeval timeout;
    timeout.tv_sec = seconds;
    timeout.tv_usec = 0;

    // Establecer timeout para envío y recepción
    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        Logger::log("Failed to set receive timeout: " + std::string(strerror(errno)), 
                    Logger::LogType::ERROR);
        return false;
    }

    if (setsockopt(socket_fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        Logger::log("Failed to set send timeout: " + std::string(strerror(errno)), 
                    Logger::LogType::ERROR);
        return false;
    }

<<<<<<< Updated upstream
=======
>>>>>>> Stashed changes
>>>>>>> Stashed changes
    return true;
}

void proxyServer::Socket::closeSocket() {
    if (socket_fd != -1) {
        close(socket_fd);
        Logger::log("Socket closed: " + std::to_string(socket_fd), 
                    Logger::LogType::SUCCESS);
        socket_fd = -1;
        running = false;
    }
}
