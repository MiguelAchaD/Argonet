#include "Handler.hpp"

proxyServer::Handler::Handler() {
    proxyServer::Handler::m_accepter_pool;
    proxyServer::Handler::m_forwarder_pool;
    proxyServer::Handler::m_sender_pool;
}

proxyServer::Handler::~Handler() {

}

bool proxyServer::Handler::addToPool(proxyServer::Handler::Type t_type) {
    return false;
}

bool proxyServer::Handler::removeFromPool(proxyServer::Handler::Type t_type) {
    return false;
}
