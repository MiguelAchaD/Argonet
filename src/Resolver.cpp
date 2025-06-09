#include "Resolver.hpp"
#include <iostream>

proxyServer::Resolver::Resolver() {

}

proxyServer::Resolver::~Resolver() {

}

proxyServer::petitionPacket proxyServer::Resolver::execute(proxyServer::petitionPacket t_packet) {
    if (!t_packet.isAccepted) {
        t_packet.isResolved = false;
        return t_packet;
    }

    t_packet.isResolved = true;
    return t_packet;
}
