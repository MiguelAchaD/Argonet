#include "Command.hpp"
#include "Logger.hpp"

#include <unistd.h>
#include <ctime>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <cstring>
#include <csignal>

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

void proxyServer::Command::setupConfigurationFile(int t_sign) {
    if (t_sign == SIGUSR1) {
        proxyServer::Logger::log("Received configuration change signal, reconfiguring", proxyServer::Logger::LogType::LOG);
        proxyServer::Command::applyConfiguration();
    }
}

nlohmann::json proxyServer::Command::readConfigurationFile() {
    std::string configuration_file = getCurrentDir() + "/configuration.json";
    std::cout << configuration_file << "\n";
    std::ifstream file(configuration_file);
    nlohmann::json config;
    if (file.is_open()) {
        file >> config;
    } else {
        proxyServer::Logger::log("No se pudo abrir el archivo de configuración", proxyServer::Logger::LogType::ERROR);
        return NULL;
    }
    return config; 
}

bool proxyServer::Command::validateConfigurationFile(nlohmann::json& t_configuration) {
    if (t_configuration == nullptr || t_configuration == NULL) {
        return false;
    }
    return false;
}

bool proxyServer::Command::saveApis(nlohmann::json& t_apis) {
    
}

bool proxyServer::Command::applyConfiguration() {
    proxyServer::Logger::log("Applying configuration", proxyServer::Logger::LogType::LOG);
    nlohmann::json result = readConfigurationFile();
    bool is_valid = validateConfigurationFile(result);
    if (!is_valid) {
        proxyServer::Logger::log("Configuration file is not valid, aborting operation...", proxyServer::Logger::LogType::ERROR);
        return false;
    }
    
    return true;
}

void proxyServer::Command::openDatabase(sqlite3*& t_db) {
    int rc = sqlite3_open("db.db", &t_db); 
    if (rc) {
        proxyServer::Logger::log("Opening the database: " + std::string(sqlite3_errmsg(t_db)), proxyServer::Logger::LogType::ERROR);
        exit(1);
    }
    proxyServer::Logger::log("Database opened", proxyServer::Logger::LogType::LOG);
}

void proxyServer::Command::closeDatabase(sqlite3* t_db) {
    sqlite3_close(t_db);  // Ya que t_db es un puntero
}

void proxyServer::Command::formatDatabase(sqlite3 &t_db) {
    //TODO: Create a sqlite file with a default configuration
}
