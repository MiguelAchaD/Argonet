#pragma once

#include "Socket.hpp"
#include "Packet.hpp"
#include "Blacklist.hpp"
#include <string>

namespace proxyServer {

class Accepter : public Socket {
public:
    Accepter(unsigned short int t_port_number) : Socket(t_port_number) {}
    ~Accepter() = default;

    petitionPacket execute(int t_client_socket);
    std::string readContents(int t_client_socket);
    petitionPacket parseContents(std::string t_result);

private:
    std::string extractHost(const std::string& request);
};

} // namespace proxyServer
