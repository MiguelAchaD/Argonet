#pragma once

#include "Socket.hpp"

namespace proxyServer {
    class Sender : proxyServer::Socket {
    protected:
        void sendToClient();
    };
}
