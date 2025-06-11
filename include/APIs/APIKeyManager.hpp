#pragma once

#include <string>
#include <vector>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include "Logger.hpp"

namespace proxyServer {

class APIKeyManager {
public:
    static APIKeyManager& getInstance() {
        static APIKeyManager instance;
        return instance;
    }

    void loadFromConfig(const std::string& configPath) {
        try {
            std::ifstream file(configPath);
            if (!file.is_open()) {
                throw std::runtime_error("Could not open config file: " + configPath);
            }

            nlohmann::json config;
            file >> config;

            std::lock_guard<std::mutex> lock(m_mutex);
            m_keys.clear();

            // Load API keys from configuration
            if (config.contains("virustotal") && 
                config["virustotal"].contains("api_keys") && 
                config["virustotal"]["api_keys"].is_array()) {
                
                for (const auto& key : config["virustotal"]["api_keys"]) {
                    m_keys.push_back(key.get<std::string>());
                }
            }

            Logger::log("API keys configuration updated successfully.", Logger::LogType::SUCCESS);
            Logger::log("Loaded " + std::to_string(m_keys.size()) + " API keys", 
                       Logger::LogType::LOG);
        } catch (const std::exception& e) {
            Logger::log("Error loading API keys config: " + std::string(e.what()), 
                       Logger::LogType::ERROR);
            throw;
        }
    }

    std::vector<std::string> getKeys() const {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_keys;
    }

private:
    APIKeyManager() = default;
    ~APIKeyManager() = default;
    APIKeyManager(const APIKeyManager&) = delete;
    APIKeyManager& operator=(const APIKeyManager&) = delete;

    std::vector<std::string> m_keys;
    mutable std::mutex m_mutex;
};

} // namespace proxyServer 