#include "Handler.hpp"
#include "Logger.hpp"

proxyServer::Handler::Handler() 
    : m_accepter_pool(), m_forwarder_pool(), m_sender_pool()
{
    proxyServer::Logger::log("Initialasing Handler...", proxyServer::Logger::LogType::LOG);

    proxyServer::Handler::m_port_pool.push_back(82);
    proxyServer::Handler::m_port_pool.push_back(83);

    unsigned short int port1 = pickPort();
    unsigned short int port2 = pickPort();
    if (port1 == 0 || port2 == 0) {
        proxyServer::Logger::log("No ports available for Handler initialisation, aborting...", proxyServer::Logger::LogType::ERROR);
        exit(1);
    }

    auto forwarder = std::make_unique<proxyServer::Forwarder>(port1);
    auto sender = std::make_unique<proxyServer::Sender>(port2);

    proxyServer::Handler::m_forwarder_pool.addToPool(std::move(forwarder));
    proxyServer::Handler::m_sender_pool.addToPool(std::move(sender));

    proxyServer::Logger::log("Initialased Handler", proxyServer::Logger::LogType::SUCCESS);
}

proxyServer::Handler::~Handler() {

}

unsigned short int proxyServer::Handler::pickPort() {
    int pool_size = proxyServer::Handler::m_port_pool.size();
    if (pool_size > 0) {
        unsigned short int selected_port = proxyServer::Handler::m_port_pool[pool_size - 1];
        proxyServer::Handler::m_port_pool.pop_back();
        proxyServer::Handler::m_port_busy_pool.push_back(selected_port);
        return selected_port;
    }
    return 0;
}
