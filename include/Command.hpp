#pragma once

#include <string>
#include <unordered_map>
#include "nlohmann/json.hpp"

namespace proxyServer {
<<<<<<< Updated upstream
    class Command {
=======
<<<<<<< Updated upstream
class Command {
=======
<<<<<<< Updated upstream
    class Command {
=======
class Command {
    private:
        static nlohmann::json readConfigurationFile();
        static bool validateConfigurationFile(nlohmann::json& t_configuration);
        static bool saveApis(nlohmann::json& t_apis);

>>>>>>> Stashed changes
>>>>>>> Stashed changes
>>>>>>> Stashed changes
    public:
        static std::string getCurrentTimestamp();
        static std::string getCurrentDir();
        static std::string getActiveInterfaceIP();
<<<<<<< Updated upstream
    };
=======
<<<<<<< Updated upstream
};
=======
<<<<<<< Updated upstream
    };
=======
        static void setupConfigurationFile(int t_sign);
        static bool applyConfiguration();
};
>>>>>>> Stashed changes
>>>>>>> Stashed changes
>>>>>>> Stashed changes
}
