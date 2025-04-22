#include "Resolver.hpp"

proxyServer::Resolver::Resolver() {

}

proxyServer::Resolver::~Resolver() {

}

proxyServer::petitionPacket proxyServer::Resolver::execute(proxyServer::petitionPacket t_packet) {
    t_packet.isResolved = true;
    return t_packet;
}
