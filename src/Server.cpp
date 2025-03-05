#include "Server.hpp"
#include "Command.hpp"
#include "Logger.hpp"

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h> 

proxyServer::Server::Server(unsigned short int t_port_number)
        : proxyServer::Socket(t_port_number),
        m_ip_address(proxyServer::Command::getActiveInterfaceIP()),
        handler()
{
    if (m_ip_address.empty()) {
        proxyServer::Logger::log("Failed to get active interface IP", proxyServer::Logger::LogType::ERROR);
        exit(1);
    } else {
        proxyServer::Logger::log("Server initialized with IP: " + m_ip_address, proxyServer::Logger::LogType::SUCCESS);
    }

    bool result = initialiseServerSocket();
    if (!result) {
        proxyServer::Logger::log("Failed to initialise Server Socket", proxyServer::Logger::LogType::ERROR);
        exit(1);
    } else {
        proxyServer::Logger::log("Initialised Server Socket successfully", proxyServer::Logger::LogType::SUCCESS);
    }

    proxyServer::Socket::configureListener();
}

proxyServer::Server::~Server() {
    proxyServer::Socket::running = false;
}

void proxyServer::Server::startServer() {
    proxyServer::Socket::startListener(proxyServer::Socket::ExecutionType::DEFERRED);
}

bool proxyServer::Server::initialiseServerSocket() {
    proxyServer::Logger::log("Initializing Server Socket...", proxyServer::Logger::LogType::LOG);
    bool result = proxyServer::Socket::initialiseSocket();
    return result;
}

void proxyServer::Server::listenLoop() {
    while (proxyServer::Socket::running) {
        int client_socket;
        struct sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);

        client_socket = accept(socket_fd, (struct sockaddr *)&client_address, &client_len);
        if (client_socket < 0) {
            if (!proxyServer::Socket::running) break;
            continue;
        }

        proxyServer::Logger::log("New client connected! Delegating to handler", proxyServer::Logger::LogType::SUCCESS);

        proxyServer::Server::handler;
        std::thread client_thread([client_socket]() {
            char buffer[1024] = {0};
            ssize_t bytes_received = read(client_socket, buffer, 1024);
            if (bytes_received > 0) {
                std::cout << "Client says: " << buffer << std::endl;
            }
            close(client_socket);
        });

        client_thread.detach();
    }
}
