#include "Sender.hpp"
#include "Logger.hpp"
#include "Command.hpp"

#include <unistd.h>
#include <cstring>

namespace proxyServer {

Sender::Sender(unsigned short int t_port_number) : Socket(t_port_number) {}

Sender::~Sender() {}

bool Sender::execute(petitionPacket packet) {
    if (packet.client_socket <= 0) {
        Logger::log("Invalid client socket");
        return false;
    }

    if (!packet.isAccepted) {
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
