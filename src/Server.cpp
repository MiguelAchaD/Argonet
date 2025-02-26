#include "Server.hpp"
#include "Command.hpp"

proxyServer::Server::Server() {
    proxyServer::Server::m_ip_address = proxyServer::Command::getActiveInterfaceIP();
}

proxyServer::Server::~Server() {

}

bool proxyServer::Server::initialiseServer() {

    return false;
}

void proxyServer::Server::serverLoop() {

}
