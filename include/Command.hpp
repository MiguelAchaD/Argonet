#pragma once

#include <string>

namespace proxyServer {
    class Command {
    public:
        static std::string getCurrentTimestamp();
        static std::string getCurrentDir();
        static std::string getActiveInterfaceIP();
    };
}
