#pragma once

#include <string>

#include "Socket.hpp"

namespace proxyServer {
    class Accepter : proxyServer::Socket {
    private:
        std::string translateContents();
        void accept();
        void reject();
    protected:
        void examineContents();
    };
}
