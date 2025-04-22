#include "Server.hpp"
#include "Logger.hpp"

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <thread>
#include <chrono>
#include <fcntl.h>

proxyServer::Server::Server(unsigned short int t_port_number) 
    : Socket(t_port_number),
      m_ip_address(),
      m_is_running(false)
{
    proxyServer::Logger::log("Initialising Server on port \"" + std::to_string(port_number) + "\"...", proxyServer::Logger::LogType::LOG);
    initialize(port_number);
    proxyServer::Logger::log("Server initialized with IP: " + m_ip_address + " on port " + std::to_string(port_number), 
                proxyServer::Logger::LogType::SUCCESS);
}

proxyServer::Server::~Server() {
    stop();
}

<<<<<<< Updated upstream
bool proxyServer::Server::initialize(unsigned short int port_number) {
=======
<<<<<<< Updated upstream
void proxyServer::Server::startServer() {
    proxyServer::Socket::startListener(proxyServer::Socket::ExecutionType::DEFERRED);
=======
bool proxyServer::Server::initialize(unsigned short int port_number) {
    proxyServer::Command::openDatabase(*proxyServer::Server::db);

>>>>>>> Stashed changes
    m_ip_address = proxyServer::Command::getActiveInterfaceIP();
    if (m_ip_address.empty()) {
        proxyServer::Logger::log("Failed to get active interface IP", proxyServer::Logger::LogType::ERROR);
        return false;
    }

    port_number = port_number;

    if (!createSocket()) {
        proxyServer::Logger::log("Failed to create server socket", proxyServer::Logger::LogType::ERROR);
        return false;
    }

    int opt = 1;
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        proxyServer::Logger::log("Failed to set socket options", proxyServer::Logger::LogType::ERROR);
        return false;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_number);

    if (bind(socket_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        proxyServer::Logger::log("Bind failed: " + std::string(strerror(errno)), proxyServer::Logger::LogType::ERROR);
        return false;
    }

    if (listen(socket_fd, 3) < 0) {
        proxyServer::Logger::log("Listen failed: " + std::string(strerror(errno)), proxyServer::Logger::LogType::ERROR);
        return false;
    }

    fcntl(socket_fd, F_SETFL, O_NONBLOCK);

    return true;
<<<<<<< Updated upstream
=======
>>>>>>> Stashed changes
>>>>>>> Stashed changes
}

void proxyServer::Server::start() {
    if (m_is_running) return;

    m_is_running = true;
    proxyServer::Logger::log("Starting server...", proxyServer::Logger::LogType::LOG);

    while (m_is_running) {
        handler.checkFutures();

        struct sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);

        int client_socket = accept(socket_fd, (struct sockaddr *)&client_address, &client_len);
        
        if (client_socket > 0) {
            proxyServer::Logger::log("New client connected! Delegating to handler", proxyServer::Logger::LogType::SUCCESS);
            handler.accepterInvoke(client_socket);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}

void proxyServer::Server::stop() {
    m_is_running = false;
    closeSocket();
}

bool proxyServer::Server::isRunning() const {
    return m_is_running;
}

std::string proxyServer::Server::getIPAddress() const {
    return m_ip_address;
}
