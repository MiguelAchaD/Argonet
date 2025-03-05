#include "Forwarder.hpp"
#include "Logger.hpp"

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h> 

proxyServer::Forwarder::Forwarder(unsigned short int t_port_number)
        : proxyServer::Socket(t_port_number)
{
    proxyServer::Socket::configureListener();
}

proxyServer::Forwarder::~Forwarder() {
    proxyServer::Socket::running = false;
}

std::string proxyServer::Forwarder::resolveDns() {
    return "";
}

std::string proxyServer::Forwarder::fechContents() {
    return "";
}

std::string proxyServer::Forwarder::fetch() {
    return "";
}

void proxyServer::Forwarder::listenLoop() {
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

        /*handler*/
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
