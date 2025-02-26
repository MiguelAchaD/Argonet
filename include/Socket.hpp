#pragma once

namespace proxyServer {
    class Socket {
    protected:
        unsigned short int port_number;
    public:
        Socket();
        ~Socket();
    };
}
