#include "Resolver.hpp"
#include "APIs/VirustotalAPI.hpp"
#include "APIs/APIKeyManager.hpp"
#include "Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

namespace proxyServer {

Resolver::Resolver() {
    try {
        // Initialize with default API keys
        m_vt_api = std::make_unique<VirustotalAPI>("");
    } catch (const std::exception& e) {
        Logger::log("Failed to initialize Resolver: " + std::string(e.what()), 
                   Logger::LogType::ERROR);
        throw;
    }
}

Resolver::~Resolver() = default;

std::string Resolver::extractHost(const std::string& url) {
    std::regex host_pattern("^(?:https?://)?([^/:]+)");
    std::smatch matches;
    if (std::regex_search(url, matches, host_pattern)) {
        return matches[1].str();
    }
    return url;
}

petitionPacket Resolver::execute(petitionPacket t_packet) {
    if (!t_packet.isAccepted) {
        t_packet.isResolved = false;
        
        // Read the resolver.html file
        std::ifstream file("assets/static/resolver.html");
        if (file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            t_packet.response = buffer.str();
        }
        
        return t_packet;
    }

    try {
        // Check if VirusTotal API is enabled
        if (!APIKeyManager::getInstance().isEnabled()) {
            t_packet.isResolved = true;
            return t_packet;
        }

        // Extract host from the URL
        std::string host = extractHost(t_packet.host.c_str());
        Logger::log("Checking host: " + host, Logger::LogType::LOG);
        
        // Check if we have any API keys available
        auto keys = APIKeyManager::getInstance().getKeys();
        if (keys.empty()) {
            Logger::log("No API keys available for VirusTotal", Logger::LogType::WARNING);
            t_packet.isResolved = true; // Allow access if no API keys available
            return t_packet;
        }
        
        // Create a new VirusTotal API instance for this host
        VirustotalAPI vt_api(host);
        
        // Add all available API keys
        for (const auto& key : keys) {
            vt_api.addKey(key);
        }
        
        try {
            // Call the API
            std::string response = vt_api.callApi();
            
            // Try to parse the response
            try {
                nlohmann::json json_response = nlohmann::json::parse(response);
                
                // Check if the response has the expected structure
                if (!json_response.contains("data") || !json_response["data"].contains("attributes")) {
                    throw std::runtime_error("Unexpected API response format");
                }
                
                const auto& attributes = json_response["data"]["attributes"];
                if (!attributes.contains("last_analysis_stats")) {
                    throw std::runtime_error("Missing analysis stats in API response");
                }
                
                const auto& stats = attributes["last_analysis_stats"];
                if (!stats.contains("malicious") || !stats.contains("suspicious")) {
                    throw std::runtime_error("Missing required stats in API response");
                }
                
                if (stats["malicious"].get<int>() > 0 || stats["suspicious"].get<int>() > 0) {
                    // If malicious or suspicious, block access
                    t_packet.isResolved = false;
                    Logger::log("Access blocked: " + host + " is malicious/suspicious", 
                               Logger::LogType::WARNING);
                } else {
                    // If clean, allow access
                    t_packet.isResolved = true;
                    Logger::log("Access allowed: " + host + " is clean", 
                               Logger::LogType::SUCCESS);
                }
            } catch (const nlohmann::json::parse_error& e) {
                Logger::log("Failed to parse VirusTotal API response for " + host + 
                           ": " + std::string(e.what()), Logger::LogType::ERROR);
                t_packet.isResolved = true; // Allow access if response parsing fails
            } catch (const std::exception& e) {
                Logger::log("Invalid VirusTotal API response format for " + host + 
                           ": " + std::string(e.what()), Logger::LogType::ERROR);
                t_packet.isResolved = true; // Allow access if response format is invalid
            }
        } catch (const std::exception& e) {
            // If API call fails, log the error but allow access
            Logger::log("VirusTotal API call failed for " + host + ": " + std::string(e.what()), 
                       Logger::LogType::ERROR);
            t_packet.isResolved = true; // Allow access if API call fails
        }
    } catch (const std::exception& e) {
        Logger::log("Error processing host " + std::string(t_packet.host.c_str()) + 
                   ": " + std::string(e.what()), Logger::LogType::ERROR);
        t_packet.isResolved = true; // Allow access if there's an error
    }
  
    return t_packet;
}

}
