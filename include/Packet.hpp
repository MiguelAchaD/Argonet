#pragma once

#include <string>

namespace proxyServer {
struct petitionPacket {
    std::string host;
    std::string user_agent;
    std::string accept;
    std::string accept_language;
    std::string accept_encoding;
    std::string connection_type;
    std::string pragma;
    std::string cache;
    unsigned short int client_socket;
    bool isAccepted = false;
    bool isResolved = false;
    bool isForwarder = false;
    std::string response;

    bool isEmpty() {
        if (host.empty()) {
            return true;
        }
        return false;
    }

    std::string toString() const{
        std::string result = std::string("GET / HTTP/1.1\n") +
                             "Host: " + host + "\n" +
                             "User-Agent: ";
        
        if (user_agent.empty()) {
            result += "Mozilla/5.0";
        } else {
            result += user_agent;
        }

        result += "\nAccept: ";
        if (accept.empty()) {
            result += "text/html\n";
        } else {
            result += accept + "\n";
        }
        
        if (!accept_language.empty()) {
            result += "Accept-Language: " + accept_language + "\n";
        }

        if (!accept_encoding.empty()) {
            result += "Accept-Encoding: " + accept_encoding + "\n";
        }

        if (!connection_type.empty()) {
            result += "Connection: " + connection_type + "\n";
        }

        if (!pragma.empty()) {
            result += "Pragma: " + pragma + "\n";
        }

        if (!cache.empty()) {
            result += "Cache-Control: " + cache + "\n";
        }
        result += "\n";

        return result;
    }
};
}
