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
        // Load API keys from configuration
        APIKeyManager::getInstance().loadFromConfig("configuration.json");
        for (const auto& key : APIKeyManager::getInstance().getKeys()) {
            m_vt_api->addKey(key);
        }
        Logger::log("Resolver initialized successfully", Logger::LogType::SUCCESS);
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
        return t_packet;
    }

    try {
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
            vt_api.callApi();
            
            if (vt_api.m_scan_malicious > 0 || vt_api.m_scan_suspicious > 0) {
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
