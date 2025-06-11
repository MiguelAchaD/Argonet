#pragma once

#include <string>
#include <vector>
#include <regex>
#include <memory>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "Logger.hpp"

namespace proxyServer {

class Blacklist {
public:
    static Blacklist& getInstance() {
        static Blacklist instance;
        return instance;
    }

    bool isAllowed(const std::string& host) const {
        // If blacklist is disabled, allow all hosts
        if (!m_enabled) {
            return true;
        }

        // Check cache first
        auto it = m_cache.find(host);
        if (it != m_cache.end()) {
            return it->second;
        }

        // Check against patterns - if any pattern matches, block the host
        for (const auto& pattern : m_patterns) {
            if (std::regex_match(host, pattern)) {
                m_cache[host] = false;  // Block if pattern matches
                return false;
            }
        }

        m_cache[host] = true;  // Allow if no patterns match
        return true;
    }

    void loadFromConfig(const std::string& configPath) {
        try {
            std::ifstream file(configPath);
            if (!file.is_open()) {
                throw std::runtime_error("Could not open config file: " + configPath);
            }

            nlohmann::json config;
            file >> config;

            m_patterns.clear();
            m_cache.clear();

            // Set enabled flag
            m_enabled = config["blacklist"]["enabled"].get<bool>();

            for (const auto& pattern : config["blacklist"]["patterns"]) {
                m_patterns.emplace_back(pattern.get<std::string>());
            }

            Logger::log("BlackList configuration updated successfully.", Logger::LogType::SUCCESS);
            Logger::log("Blacklist is " + std::string(m_enabled ? "enabled" : "disabled") + 
                       " with " + std::to_string(m_patterns.size()) + " patterns", 
                       Logger::LogType::LOG);
        } catch (const std::exception& e) {
            Logger::log("Error loading blacklist config: " + std::string(e.what()), 
                       Logger::LogType::ERROR);
            throw;
        }
    }

private:
    Blacklist() : m_enabled(false) {}
    ~Blacklist() = default;
    Blacklist(const Blacklist&) = delete;
    Blacklist& operator=(const Blacklist&) = delete;

    std::vector<std::regex> m_patterns;
    mutable std::unordered_map<std::string, bool> m_cache;  // Cache for host lookups
    bool m_enabled;  // Whether the blacklist is enabled
};

} // namespace proxyServer 