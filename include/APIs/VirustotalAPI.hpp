#pragma once

#include "Api.hpp"

namespace proxyServer {
class VirustotalAPI : private Api {
private:

public:
    VirustotalAPI(const std::string& t_url);
    ~VirustotalAPI();

    std::string callApi() override;

    unsigned short int m_scan_malicious;
    unsigned short int m_scan_suspicious;
    unsigned short int m_scan_undetected;
    unsigned short int m_scan_harmless;
    unsigned short int m_scan_timeout;
    unsigned short int m_votes_malicious;
    unsigned short int m_votes_harmless;
    char m_country_code[3];
    unsigned short int m_creation_date;
    unsigned short int m_reputation;
};
}
