#include "Command.hpp"
#include "Logger.hpp"

#include <unistd.h>
#include <ctime>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <cstring>
#include <fstream>
#include <filesystem>

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

static void openDatabase(sqlite3*& t_db) {
    int rc = sqlite3_open("proxy.db", &t_db);
    if (rc) {
        proxyServer::Logger::log("Can't open database: " + std::string(sqlite3_errmsg(t_db)), 
                               proxyServer::Logger::LogType::ERROR);
        sqlite3_close(t_db);
        t_db = nullptr;
    } else {
        proxyServer::Logger::log("Opened database successfully", 
                               proxyServer::Logger::LogType::SUCCESS);
    }
}

static void closeDatabase(sqlite3* t_db) {
    if (t_db) {
        sqlite3_close(t_db);
        proxyServer::Logger::log("Database closed", 
                               proxyServer::Logger::LogType::SUCCESS);
    }
}

void proxyServer::Command::openDatabase(sqlite3*& t_db) {
    ::openDatabase(t_db);
}

void proxyServer::Command::closeDatabase(sqlite3* t_db) {
    ::closeDatabase(t_db);
}

void proxyServer::Command::formatDatabase(sqlite3& t_db) {
    const char* sql = 
        "DROP TABLE IF EXISTS apis;"
        "CREATE TABLE apis ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "name TEXT NOT NULL,"
        "url TEXT NOT NULL,"
        "method TEXT NOT NULL,"
        "headers TEXT,"
        "body TEXT,"
        "created_at TEXT NOT NULL,"
        "updated_at TEXT NOT NULL"
        ");";

    char* errMsg = nullptr;
    int rc = sqlite3_exec(&t_db, sql, nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        Logger::log("SQL error: " + std::string(errMsg), Logger::LogType::ERROR);
        sqlite3_free(errMsg);
    } else {
        Logger::log("Database formatted successfully", Logger::LogType::SUCCESS);
    }
}

void proxyServer::Command::setupConfigurationFile(int t_sign) {
    std::string configPath = "configuration.json";
    
    if (t_sign == 0) {
        // Create default configuration
        nlohmann::json config = {
            {"blacklist", {
                {"patterns", {
                    ".*\\.example\\.com$",
                    ".*\\.blocked\\.org$"
                }}
            }},
            {"server", {
                {"port", 8081},
                {"max_connections", 100},
                {"timeout", 30}
            }},
            {"logging", {
                {"level", "info"},
                {"file", "proxy.log"}
            }}
        };

        std::ofstream file(configPath);
        if (file.is_open()) {
            file << config.dump(4);
            file.close();
            Logger::log("Configuration file created successfully", Logger::LogType::SUCCESS);
        } else {
            Logger::log("Failed to create configuration file", Logger::LogType::ERROR);
        }
    } else {
        // Remove configuration file
        if (std::filesystem::exists(configPath)) {
            std::filesystem::remove(configPath);
            Logger::log("Configuration file removed", Logger::LogType::SUCCESS);
        }
    }
}

bool proxyServer::Command::applyConfiguration() {
    try {
        nlohmann::json config = readConfigurationFile();
        if (!validateConfigurationFile(config)) {
            return false;
        }

        // Apply configuration settings
        // TODO: Implement actual configuration application logic
        Logger::log("Configuration applied successfully", Logger::LogType::SUCCESS);
        return true;
    } catch (const std::exception& e) {
        Logger::log("Failed to apply configuration: " + std::string(e.what()), Logger::LogType::ERROR);
        return false;
    }
}

nlohmann::json proxyServer::Command::readConfigurationFile() {
    std::ifstream file("configuration.json");
    if (!file.is_open()) {
        throw std::runtime_error("Could not open configuration file");
    }

    nlohmann::json config;
    file >> config;
    return config;
}

bool proxyServer::Command::validateConfigurationFile(nlohmann::json& t_configuration) {
    try {
        // Check required sections
        if (!t_configuration.contains("blacklist") || 
            !t_configuration.contains("server") || 
            !t_configuration.contains("logging")) {
            Logger::log("Missing required configuration sections", Logger::LogType::ERROR);
            return false;
        }

        // Validate blacklist section
        if (!t_configuration["blacklist"].contains("patterns") || 
            !t_configuration["blacklist"]["patterns"].is_array()) {
            Logger::log("Invalid blacklist configuration", Logger::LogType::ERROR);
            return false;
        }

        // Validate server section
        if (!t_configuration["server"].contains("port") || 
            !t_configuration["server"].contains("max_connections") || 
            !t_configuration["server"].contains("timeout")) {
            Logger::log("Invalid server configuration", Logger::LogType::ERROR);
            return false;
        }

        // Validate logging section
        if (!t_configuration["logging"].contains("level") || 
            !t_configuration["logging"].contains("file")) {
            Logger::log("Invalid logging configuration", Logger::LogType::ERROR);
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        Logger::log("Configuration validation failed: " + std::string(e.what()), Logger::LogType::ERROR);
        return false;
    }
}

bool proxyServer::Command::saveApis(nlohmann::json& t_apis) {
    sqlite3* db;
    ::openDatabase(db);
    if (!db) {
        return false;
    }

    bool success = true;
    sqlite3_exec(db, "BEGIN TRANSACTION", nullptr, nullptr, nullptr);

    for (const auto& api : t_apis) {
        const char* sql = "INSERT INTO apis (name, url, method, headers, body, created_at, updated_at) "
                         "VALUES (?, ?, ?, ?, ?, ?, ?)";
        
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_text(stmt, 1, api["name"].get<std::string>().c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, api["url"].get<std::string>().c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, api["method"].get<std::string>().c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 4, api["headers"].dump().c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 5, api["body"].dump().c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 6, getCurrentTimestamp().c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 7, getCurrentTimestamp().c_str(), -1, SQLITE_STATIC);

            if (sqlite3_step(stmt) != SQLITE_DONE) {
                proxyServer::Logger::log("Failed to save API: " + std::string(sqlite3_errmsg(db)), 
                                       proxyServer::Logger::LogType::ERROR);
                success = false;
            }
            sqlite3_finalize(stmt);
        } else {
            proxyServer::Logger::log("Failed to prepare statement: " + std::string(sqlite3_errmsg(db)), 
                                   proxyServer::Logger::LogType::ERROR);
            success = false;
        }
    }

    if (success) {
        sqlite3_exec(db, "COMMIT", nullptr, nullptr, nullptr);
        proxyServer::Logger::log("APIs saved successfully", 
                               proxyServer::Logger::LogType::SUCCESS);
    } else {
        sqlite3_exec(db, "ROLLBACK", nullptr, nullptr, nullptr);
        proxyServer::Logger::log("Failed to save APIs", 
                               proxyServer::Logger::LogType::ERROR);
    }

    ::closeDatabase(db);
    return success;
}
