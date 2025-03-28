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
}

bool proxyServer::Sender::sendToClient(const proxyServer::petitionPacket packet) {
    if (packet.client_socket == -1) {
        Logger::log("Cannot send: Invalid client socket", Logger::LogType::ERROR);
        return false;
    }

    if (packet.response.empty()) {
        Logger::log("Empty response to send", Logger::LogType::WARNING);
        return false;
    }

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    
    if (setsockopt(packet.client_socket, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout)) < 0) {
        Logger::log("Failed to set send timeout", Logger::LogType::ERROR);
        closeClientConnection(packet.client_socket);
        return false;
    }

    ssize_t bytes_sent = send(packet.client_socket, 
                               packet.response.c_str(), 
                               packet.response.length(), 
                               0);
    
    if (bytes_sent == -1) {
        Logger::log("Failed to send response: " + std::string(strerror(errno)), 
                    Logger::LogType::ERROR);
        closeClientConnection(packet.client_socket);
        return false;
    }

    closeClientConnection(packet.client_socket);
    return true;
}

void proxyServer::Sender::closeClientConnection(int client_socket) {
    if (client_socket != -1) {
        close(client_socket);
        Logger::log("Client connection closed: " + std::to_string(client_socket), 
                    Logger::LogType::SUCCESS);
    }
}
