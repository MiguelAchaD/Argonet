#include "Sender.hpp"
#include "Logger.hpp"
#include "Command.hpp"

#include <unistd.h>
#include <cstring>
#include <fstream>
#include <sstream>

namespace proxyServer {

Sender::Sender(unsigned short int t_port_number) : Socket(t_port_number) {}

Sender::~Sender() {}

bool Sender::execute(petitionPacket packet) {
    if (packet.client_socket <= 0) {
        Logger::log("Invalid client socket");
        return false;
    }

    if (!packet.isAccepted) {
        // Read blacklist HTML file
        std::ifstream file("assets/static/blacklist.html");
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string html_content = buffer.str();

        // Create HTTP response with the blacklist HTML
        std::string response = "HTTP/1.1 403 Forbidden\r\n";
        response += "Content-Type: text/html\r\n";
        response += "Content-Length: " + std::to_string(html_content.length()) + "\r\n";
        response += "Connection: close\r\n\r\n";
        response += html_content;

        // Send response to client
        ssize_t bytes_sent = send(packet.client_socket, response.c_str(), response.size(), 0);
        if (bytes_sent < 0) {
            Logger::log("Failed to send blacklist response to client", Logger::LogType::ERROR);
        }

        // Close client socket
        close(packet.client_socket);

        Logger::log("[" + std::string(Command::getCurrentTimestamp()) + "] - Petition to " + packet.host.c_str() + " finished -- Accepted: " + std::to_string(packet.isAccepted) + " - Resolved: " + std::to_string(packet.isResolved) + " - Forwarder: " + std::to_string(packet.isForwarder), Logger::LogType::LOG);
        return false;
    } else if (!packet.isResolved) {
        // Read resolver HTML file
        std::ifstream file("assets/static/resolver.html");
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string html_content = buffer.str();

        // Create HTTP response with the resolver HTML
        std::string response = "HTTP/1.1 403 Forbidden\r\n";
        response += "Content-Type: text/html\r\n";
        response += "Content-Length: " + std::to_string(html_content.length()) + "\r\n";
        response += "Connection: close\r\n\r\n";
        response += html_content;

        // Send response to client
        ssize_t bytes_sent = send(packet.client_socket, response.c_str(), response.size(), 0);
        if (bytes_sent < 0) {
            Logger::log("Failed to send resolver response to client", Logger::LogType::ERROR);
        }

        // Close client socket
        close(packet.client_socket);

        Logger::log("[" + std::string(Command::getCurrentTimestamp()) + "] - Petition to " + packet.host.c_str() + " finished -- Accepted: " + std::to_string(packet.isAccepted) + " - Resolved: " + std::to_string(packet.isResolved) + " - Forwarder: " + std::to_string(packet.isForwarder), Logger::LogType::LOG);
        return false;
    }

    try {
        // Send response to client
        ssize_t bytes_sent = send(packet.client_socket, packet.response.c_str(), packet.response.size(), 0);
        if (bytes_sent < 0) {
            Logger::log("Failed to send response to client", Logger::LogType::ERROR);
            return false;
        }

        // Close client socket
        close(packet.client_socket);

        Logger::log("[" + std::string(Command::getCurrentTimestamp()) + "] - Petition to " + packet.host.c_str() + " finished -- Accepted: " + std::to_string(packet.isAccepted) + " - Resolved: " + std::to_string(packet.isResolved) + " - Forwarder: " + std::to_string(packet.isForwarder), Logger::LogType::LOG);
        return true;
    } catch (const std::exception& e) {
        Logger::log("Error Sending petition to Client:" + std::string(e.what()), Logger::LogType::ERROR);
        return false;
    }
}

void Sender::closeClientConnection(int client_socket) {
    close(client_socket);
    Logger::log("Client connection closed: " + std::to_string(client_socket));
}

void Sender::disconnect() {
    closeSocket();
}

} // namespace proxyServer
