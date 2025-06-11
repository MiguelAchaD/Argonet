#pragma once

#include "Api.hpp"
#include <nlohmann/json.hpp>

namespace proxyServer {

class VirustotalAPI : public Api {
private:
    enum class ResourceType { DOMAIN, URL, IP };
    ResourceType m_resource_type;
    std::string m_resource;
    
    void parseResponse(const nlohmann::json& response);
    ResourceType determineResourceType(const std::string& resource);
    std::string buildUrl();

public:
    VirustotalAPI(const std::string& resource);
    ~VirustotalAPI() override = default;

    std::string callApi() override;

    // Results from the API call
    unsigned short int m_scan_malicious{0};
    unsigned short int m_scan_suspicious{0};
    unsigned short int m_scan_undetected{0};
    unsigned short int m_scan_harmless{0};
    unsigned short int m_scan_timeout{0};
    unsigned short int m_votes_malicious{0};
    unsigned short int m_votes_harmless{0};
    char m_country_code[3]{'\0'};
    unsigned short int m_creation_date{0};
    unsigned short int m_reputation{0};
};

}
