#include "Handler.hpp"
#include "Logger.hpp"

#include <thread>
#include <chrono>
#include <unistd.h> 

proxyServer::Handler::Handler() 
    : m_accepter_pool(),
      m_forwarder_pool(),
      m_sender_pool(),
      m_resolver_pool(){

    proxyServer::Logger::log("Initializing Handler...", proxyServer::Logger::LogType::LOG);
    
    m_port_pool.push_back(82);
    m_port_pool.push_back(83);

    unsigned short int port1 = pickPort();
    unsigned short int port2 = pickPort();
    
    if (port1 == 0 || port2 == 0) {
        proxyServer::Logger::log("No ports available for Handler initialization, aborting...", 
                                  proxyServer::Logger::LogType::ERROR);
        exit(1);
    }
    
    auto forwarder = std::make_unique<proxyServer::Forwarder>(port1);
    auto sender = std::make_unique<proxyServer::Sender>(port2);
    
    m_forwarder_pool.addToPool(std::move(forwarder));
    m_sender_pool.addToPool(std::move(sender));
    
    proxyServer::Logger::log("Initialized Handler", proxyServer::Logger::LogType::SUCCESS);
}

proxyServer::Handler::~Handler() {
}

void proxyServer::Handler::accepterInvoke(int t_client_socket) {
    std::unique_ptr<proxyServer::Accepter> accepter = m_accepter_pool.poolInvoke();
    std::shared_ptr<proxyServer::Accepter> sharedAccepter = std::move(accepter);
    
    std::future<proxyServer::petitionPacket> result = std::async(std::launch::async, 
        [sharedAccepter, t_client_socket]() {
            return sharedAccepter->examineContents(t_client_socket);
        }
    );
    
    accepterFutures.push_back(std::move(result));
}

void proxyServer::Handler::resolverInvoke(proxyServer::petitionPacket t_packet) {
    std::unique_ptr<proxyServer::Resolver> resolver = m_resolver_pool.poolInvoke();
    std::shared_ptr<proxyServer::Resolver> sharedResolver = std::move(resolver);
    
    std::future<proxyServer::petitionPacket> result = std::async(std::launch::async, 
        [sharedResolver, t_packet]() {
            return sharedResolver->evaluateResults(t_packet);
        }
    );
    
    resolverFutures.push_back(std::move(result));
}

void proxyServer::Handler::forwarderInvoke(proxyServer::petitionPacket t_packet) {
    std::unique_ptr<proxyServer::Forwarder> forwarder = m_forwarder_pool.poolInvoke();
    std::shared_ptr<proxyServer::Forwarder> sharedForwarder = std::move(forwarder);
    
    std::future<proxyServer::petitionPacket> result = std::async(std::launch::async, 
        [sharedForwarder, t_packet]() {
            return sharedForwarder->fetch(t_packet);
        }
    );
    
    forwarderFutures.push_back(std::move(result));
}

void proxyServer::Handler::senderInvoke(proxyServer::petitionPacket t_packet) {
    std::unique_ptr<proxyServer::Sender> sender = m_sender_pool.poolInvoke();
    std::shared_ptr<proxyServer::Sender> sharedSender = std::move(sender);
    
    std::future<void> result = std::async(std::launch::async, 
        [sharedSender, t_packet]() {
            sharedSender->sendToClient(t_packet);
        }
    );
}

template <typename FutureContainer, typename ProcessFunction>
void proxyServer::Handler::processFutures(FutureContainer& futures, ProcessFunction processFunction) {
    for (auto it = futures.begin(); it != futures.end();) {
        if (it->wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            try {
                proxyServer::petitionPacket packet = it->get();
                
                bool shouldContinue = processFunction(packet);
                
                if (shouldContinue) {
                    it = futures.erase(it);
                } else {
                    // close(packet.client_socket);
                    proxyServer::Logger::log("Invalid packet received", 
                                             proxyServer::Logger::LogType::WARNING);
                    it = futures.erase(it);
                }
            }
            catch (const std::exception& e) {
                proxyServer::Logger::log("Future Processing Error: " + std::string(e.what()), 
                                         proxyServer::Logger::LogType::ERROR);
                it = futures.erase(it);
            }
        }
        else {
            ++it;
        }
    }
}

void proxyServer::Handler::checkFutures() {
    processFutures(accepterFutures, [this](proxyServer::petitionPacket& packet) {
        if (packet.client_socket > 0 && !packet.isEmpty()) {
            resolverInvoke(packet);
            return true;
        }
        return false;
    });

    processFutures(resolverFutures, [this](proxyServer::petitionPacket& packet) {
        if (packet.client_socket > 0 && !packet.isEmpty() && packet.isResolved) {
            forwarderInvoke(packet);
            return true;
        }
        return false;
    });

    processFutures(forwarderFutures, [this](proxyServer::petitionPacket& packet) {
        if (packet.client_socket > 0 && !packet.isEmpty() && 
            packet.isResolved && !packet.response.empty()) {
            senderInvoke(packet);
            return true;
        }
        return false;
    });
}

unsigned short int proxyServer::Handler::pickPort() {
    int pool_size = m_port_pool.size();
    if (pool_size > 0) {
        unsigned short int selected_port = m_port_pool[pool_size - 1];
        m_port_pool.pop_back();
        m_port_busy_pool.push_back(selected_port);
        return selected_port;
    }
    return 0;
}
