#pragma once

#include "Packet.hpp"
#include "APIs/VirustotalAPI.hpp"
#include <memory>

namespace proxyServer {

class Resolver {
private:
    std::unique_ptr<VirustotalAPI> m_vt_api;
    std::string extractHost(const std::string& url);

public:
    Resolver();
    ~Resolver();

    petitionPacket execute(petitionPacket t_packet);
};

}
