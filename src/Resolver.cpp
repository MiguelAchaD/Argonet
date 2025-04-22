<<<<<<< Updated upstream
=======
<<<<<<< Updated upstream
=======
>>>>>>> Stashed changes
#include "Resolver.hpp"

proxyServer::Resolver::Resolver() {

}

proxyServer::Resolver::~Resolver() {

}

<<<<<<< Updated upstream
proxyServer::petitionPacket proxyServer::Resolver::evaluateResults(proxyServer::petitionPacket t_packet) {
    t_packet.isResolved = true;
    return t_packet;
}
=======
proxyServer::petitionPacket proxyServer::Resolver::execute(proxyServer::petitionPacket t_packet) {
    //Call APIs and process result with ML model
    t_packet.isResolved = true;
    return t_packet;
}
>>>>>>> Stashed changes
>>>>>>> Stashed changes
