#include "Forwarder.hpp"
#include "Logger.hpp"

#include <cstring>
#include <unistd.h>
#include <vector>

proxyServer::Forwarder::Forwarder(unsigned short int t_port_number) : Socket(t_port_number) {}

proxyServer::Forwarder::~Forwarder() {
    disconnect();
}

proxyServer::petitionPacket proxyServer::Forwarder::execute(proxyServer::petitionPacket t_packet) {
    if (connect(t_packet.host, 80)) {
        sendRequest(t_packet);
        t_packet.response = receiveResponse();
        /*disconnect();*/
    }
    return t_packet;
}

std::string proxyServer::Forwarder::sendRequest(const proxyServer::petitionPacket& packet) {
    std::string request = packet.toString();
    ssize_t bytes_sent = send(socket_fd, request.c_str(), request.length(), 0);
    
    if (bytes_sent == -1) {
        Logger::log("Send failed: " + std::string(strerror(errno)), Logger::LogType::ERROR);
        return "";
    }
    return std::to_string(bytes_sent);
}

bool proxyServer::Forwarder::connect(const std::string& host, int port) {
    struct addrinfo hints, *servinfo;
    
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    std::string port_str = std::to_string(port);
    int rv = getaddrinfo(host.c_str(), port_str.c_str(), &hints, &servinfo);
    if (rv != 0) {
        Logger::log("getaddrinfo error: " + std::string(gai_strerror(rv)), Logger::LogType::ERROR);
        return false;
    }

    if (!createSocket()) {
        freeaddrinfo(servinfo);
        return false;
    }

    setSocketTimeout(5);

    if (::connect(socket_fd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        Logger::log("Connection failed: " + std::string(strerror(errno)), Logger::LogType::ERROR);
        freeaddrinfo(servinfo);
        return false;
    }

    freeaddrinfo(servinfo);
    return true;
}

std::string proxyServer::Forwarder::receiveResponse(size_t buffer_size) {
    std::vector<char> buffer(buffer_size);
    std::string full_response;

    ssize_t bytes_received;
    while ((bytes_received = recv(socket_fd, buffer.data(), buffer_size - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        full_response.append(buffer.data());
    }

    return full_response;
}

void proxyServer::Forwarder::disconnect() {
    std::cout << "FORWARDER CLOSINGGGG\n";
    closeSocket();
}
