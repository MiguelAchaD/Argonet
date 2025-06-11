#include "Resolver.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

proxyServer::Resolver::Resolver() {

}

proxyServer::Resolver::~Resolver() {

}

proxyServer::petitionPacket proxyServer::Resolver::execute(proxyServer::petitionPacket t_packet) {
    if (!t_packet.isAccepted) {
        t_packet.isResolved = false;
        
        // Read the resolver.html file
        std::ifstream file("assets/static/resolver.html");
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            t_packet.response = buffer.str();
        }
        
        return t_packet;
    }
  
    t_packet.isResolved = true;
    return t_packet;
}
