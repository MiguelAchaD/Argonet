#include "Sender.hpp"
#include "Logger.hpp"

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h> 
#include <cstring>
#include <thread>
#include <chrono>

#include "Sender.hpp"
#include "Logger.hpp"
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h> 
#include <cstring>

proxyServer::Sender::Sender(unsigned short int t_port_number) : Socket(t_port_number) {}

proxyServer::Sender::~Sender() {
    proxyServer::Socket::running = false;
}

bool proxyServer::Sender::execute(const proxyServer::petitionPacket packet) {
    if (packet.client_socket == -1) {
        proxyServer::Logger::log("Cannot send: Invalid client socket", Logger::LogType::ERROR);
        return false;
    }

    if (packet.response.empty()) {
        proxyServer::Logger::log("Empty response to send", Logger::LogType::WARNING);
        return false;
    }

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    
    if (setsockopt(packet.client_socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        proxyServer::Logger::log("Failed to set send timeout", Logger::LogType::ERROR);
        // closeClientConnection(packet.client_socket);
        return false;
    }

    ssize_t bytes_sent = send(packet.client_socket, 
                               packet.response.c_str(), 
                               packet.response.length(), 
                               0);
    
    if (bytes_sent == -1) {
        proxyServer::Logger::log("Failed to send response: " + std::string(strerror(errno)), 
                    proxyServer::Logger::LogType::ERROR);
        // closeClientConnection(packet.client_socket);
        return false;
    }

    // closeClientConnection(packet.client_socket);
    return true;
}

void proxyServer::Sender::closeClientConnection(int client_socket) {
    close(client_socket);
    proxyServer::Logger::log("Client connection closed: " + std::to_string(client_socket), 
                proxyServer::Logger::LogType::SUCCESS);
}

void proxyServer::Sender::disconnect() {
    closeSocket();
}
