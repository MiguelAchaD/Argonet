#pragma once

#include <string>
<<<<<<< Updated upstream
#include <unordered_map>
#include "nlohmann/json.hpp"
=======
<<<<<<< Updated upstream
=======
#include <unordered_map>
#include "nlohmann/json.hpp"
#include <sqlite3.h>
>>>>>>> Stashed changes
>>>>>>> Stashed changes

namespace proxyServer {
  class Command {
    private:
        static nlohmann::json readConfigurationFile();
        static bool validateConfigurationFile(nlohmann::json& t_configuration);
        static bool saveApis(nlohmann::json& t_apis);

    public:
        // Database methods
        static void openDatabase(sqlite3 &t_db);
        static void closeDatabase(sqlite3 &t_db);
        static void formatDatabase(sqlite3 &t_db); // Use carefully!

        // Configuration methods
        static void setupConfigurationFile(int t_sign);
        static bool applyConfiguration();
    
        // Auxiliary methods
        static std::string getCurrentTimestamp();
        static std::string getCurrentDir();
        static std::string getActiveInterfaceIP();
<<<<<<< Updated upstream

        static void setupConfigurationFile(int t_sign);
        static bool applyConfiguration();
  };
=======
<<<<<<< Updated upstream
    };
=======
        static void setupConfigurationFile(int t_sign);
        static bool applyConfiguration();
>>>>>>> Stashed changes
>>>>>>> Stashed changes
}
