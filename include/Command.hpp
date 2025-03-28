#pragma once

#include <string>
#include <unordered_map>
#include "nlohmann/json.hpp"

namespace proxyServer {
  class Command {
    private:
        static nlohmann::json readConfigurationFile();
        static bool validateConfigurationFile(nlohmann::json& t_configuration);
        static bool saveApis(nlohmann::json& t_apis);

    public:
        static std::string getCurrentTimestamp();
        static std::string getCurrentDir();
        static std::string getActiveInterfaceIP();

        static void setupConfigurationFile(int t_sign);
        static bool applyConfiguration();
  };
}
