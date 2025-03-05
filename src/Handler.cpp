#include "Handler.hpp"
#include "Logger.hpp"

proxyServer::Handler::Handler() {
    proxyServer::Logger::log("Initialasing Handler...", proxyServer::Logger::LogType::LOG);
    proxyServer::Handler::m_port_pool.push_back(82);
    proxyServer::Handler::m_port_pool.push_back(83);
    proxyServer::Handler::m_accepter_pool.emplace_back(std::make_unique<Accepter>());
    unsigned short int port1 = pickPort();
    unsigned short int port2 = pickPort();
    if (port1 == 0 || port2 == 0) {
        proxyServer::Logger::log("No ports available for Handler initialisation, aborting...", proxyServer::Logger::LogType::ERROR);
        exit(1);
    }
    proxyServer::Forwarder forwarder(port1);
    proxyServer::Sender sender(port2);
    proxyServer::Handler::m_forwarder_pool.emplace_back(std::make_unique<Forwarder>(port1));
    proxyServer::Handler::m_sender_pool.emplace_back(std::make_unique<Sender>(port2));
    proxyServer::Logger::log("Initialased Handler", proxyServer::Logger::LogType::SUCCESS);
}

proxyServer::Handler::~Handler() {

}

bool proxyServer::Handler::addToPool(proxyServer::Handler::Type t_type) {
    switch (t_type) {
        case proxyServer::Handler::Type::ACCEPTER:
            break;
        case proxyServer::Handler::Type::FORWARDER:
            break;
        case proxyServer::Handler::Type::SENDER:
            break;
        default:
            proxyServer::Logger::log("An impossible type of handled type was added to a pool, cancelling operation", proxyServer::Logger::LogType::WARNING);
            return false;
            break;
    }

    return false;
}

bool proxyServer::Handler::removeFromPool(proxyServer::Handler::Type t_type) {
    bool operation_result = false;
    switch (t_type) {
        case proxyServer::Handler::Type::ACCEPTER:
        {
            int accepter_pool_size = proxyServer::Handler::m_accepter_pool.size();
            if (accepter_pool_size > 1) {

            }
            break;
        }
        case proxyServer::Handler::Type::FORWARDER:
        {

        }
            break;
        case proxyServer::Handler::Type::SENDER:
        {

        }
            break;
        default:
            proxyServer::Logger::log("An impossible type of handled type was removed to a pool, cancelling operation", proxyServer::Logger::LogType::WARNING);
            break;
    }

    return operation_result;
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
