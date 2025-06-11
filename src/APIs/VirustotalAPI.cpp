#include "VirustotalAPI.hpp"
#include <regex>
#include <iostream>
#include <array>

namespace proxyServer {

namespace {
    const std::array<char, 64> base64_chars = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_'
    };

    std::string base64url_encode(const std::string& input) {
        std::string result;
        result.reserve(((input.size() + 2) / 3) * 4);
        
        for (size_t i = 0; i < input.size(); i += 3) {
            unsigned char octet_a = i < input.size() ? input[i] : 0;
            unsigned char octet_b = i + 1 < input.size() ? input[i + 1] : 0;
            unsigned char octet_c = i + 2 < input.size() ? input[i + 2] : 0;

            unsigned int triple = (octet_a << 16) + (octet_b << 8) + octet_c;

            result.push_back(base64_chars[(triple >> 18) & 0x3F]);
            result.push_back(base64_chars[(triple >> 12) & 0x3F]);
            result.push_back(base64_chars[(triple >> 6) & 0x3F]);
            result.push_back(base64_chars[triple & 0x3F]);
        }

        // Remove padding
        while (!result.empty() && result.back() == '=') {
            result.pop_back();
        }

        return result;
    }
}

VirustotalAPI::VirustotalAPI(const std::string& resource)
    : Api("", 4), m_resource(resource) {
    m_resource_type = determineResourceType(resource);
    m_url = buildUrl();
}

VirustotalAPI::ResourceType VirustotalAPI::determineResourceType(const std::string& resource) {
    // IP address regex pattern
    std::regex ip_pattern("^\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}$");
    
    // URL regex pattern
    std::regex url_pattern("^https?://[^\\s/$.?#].[^\\s]*$");
    
    if (std::regex_match(resource, ip_pattern)) {
        return ResourceType::IP;
    } else if (std::regex_match(resource, url_pattern)) {
        return ResourceType::URL;
    } else {
        return ResourceType::DOMAIN;
    }
}

std::string VirustotalAPI::buildUrl() {
    std::string base_url = "https://www.virustotal.com/api/v3/";
    
    switch (m_resource_type) {
        case ResourceType::DOMAIN:
            return base_url + "domains/" + m_resource;
        case ResourceType::URL: {
            // For URLs, we need to encode the URL in base64url format
            std::string url_to_encode = m_resource;
            if (url_to_encode.find("://") == std::string::npos) {
                url_to_encode = "http://" + url_to_encode;
            }
            std::string encoded = base64url_encode(url_to_encode);
            return base_url + "urls/" + encoded;
        }
        case ResourceType::IP:
            return base_url + "ip_addresses/" + m_resource;
        default:
            throw std::runtime_error("Invalid resource type");
    }
}

void VirustotalAPI::parseResponse(const nlohmann::json& response) {
    try {
        const auto& data = response["data"];
        const auto& attributes = data["attributes"];
        
        // Parse scan results
        if (attributes.contains("last_analysis_stats")) {
            const auto& stats = attributes["last_analysis_stats"];
            m_scan_malicious = stats["malicious"].get<unsigned short int>();
            m_scan_suspicious = stats["suspicious"].get<unsigned short int>();
            m_scan_undetected = stats["undetected"].get<unsigned short int>();
            m_scan_harmless = stats["harmless"].get<unsigned short int>();
            m_scan_timeout = stats["timeout"].get<unsigned short int>();
        }

        // Parse votes
        if (attributes.contains("total_votes")) {
            const auto& votes = attributes["total_votes"];
            m_votes_malicious = votes["malicious"].get<unsigned short int>();
            m_votes_harmless = votes["harmless"].get<unsigned short int>();
        }

        // Parse additional info
        if (attributes.contains("country")) {
            std::string country = attributes["country"].get<std::string>();
            if (country.length() >= 2) {
                m_country_code[0] = country[0];
                m_country_code[1] = country[1];
                m_country_code[2] = '\0';
            }
        }

        if (attributes.contains("creation_date")) {
            m_creation_date = attributes["creation_date"].get<unsigned short int>();
        }

        if (attributes.contains("reputation")) {
            m_reputation = attributes["reputation"].get<unsigned short int>();
        }
    } catch (const std::exception& e) {
        std::cerr << "Error parsing VirusTotal response: " << e.what() << std::endl;
        throw;
    }
}

std::string VirustotalAPI::callApi() {
    try {
        std::string response = fetch();
        nlohmann::json json_response = nlohmann::json::parse(response);
        parseResponse(json_response);
        return response;
    } catch (const std::exception& e) {
        std::cerr << "Error calling VirusTotal API: " << e.what() << std::endl;
        throw;
    }
}

}
