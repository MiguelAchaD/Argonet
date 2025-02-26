#include "Command.hpp"
#include "Logger.hpp"

#include <unistd.h>
#include <ctime>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <cstring>

std::string proxyServer::Command::getCurrentTimestamp() {
    std::time_t now = std::time(nullptr);
    std::tm localTime;
    localtime_r(&now, &localTime);

    char buffer[20];
    std::strftime(buffer, sizeof(buffer), "%Y%m%d%H%M%S", &localTime);
    return std::string(buffer);
}

std::string proxyServer::Command::getCurrentDir() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        std::string currentDir(cwd);
        return currentDir;
    }
    return "";
}

std::string proxyServer::Command::getActiveInterfaceIP() {
    struct ifaddrs *ifAddrStruct = nullptr;
    struct ifaddrs *ifa = nullptr;
    void *tmpAddrPtr = nullptr;

    if (getifaddrs(&ifAddrStruct) == -1) {
        proxyServer::Logger::log("Error obtaining net interfaces");
        return "";
    }

    for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET && !(ifa->ifa_flags & IFF_LOOPBACK)) {
            tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;

            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

            if (strcmp(addressBuffer, "127.0.0.1") != 0) {
                std::string interfaceName = ifa->ifa_name;
                proxyServer::Logger::log("Active interface: " + interfaceName + ", IP: " + addressBuffer);

                return std::string(addressBuffer);
            }
        }
    }

    if (ifAddrStruct != nullptr) {
        freeifaddrs(ifAddrStruct);
    }

    return "";
}
