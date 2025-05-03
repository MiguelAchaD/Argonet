#pragma once

#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include <sqlite3.h>

namespace proxyServer {
  class Command {
    private:
        static nlohmann::json readConfigurationFile();
        static bool validateConfigurationFile(nlohmann::json& t_configuration);
        static bool saveApis(nlohmann::json& t_apis);

    public:
        // Database methods
        void openDatabase(sqlite3*& t_db);
        void closeDatabase(sqlite3* t_db);
        static void formatDatabase(sqlite3 &t_db); // Use carefully!

        // Configuration methods
        static void setupConfigurationFile(int t_sign);
        static bool applyConfiguration();
    
        // Auxiliary methods
        static std::string getCurrentTimestamp();
        static std::string getCurrentDir();
        static std::string getActiveInterfaceIP();
  };
}
