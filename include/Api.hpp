#pragma once

#include <curl/curl.h>
#include <string>
#include <vector>

namespace proxyServer {
class Api {
private:
    enum class OperationType { GET, POST, PUT };
    std::string m_name;
    std::string m_url;
    std::string m_key;
    OperationType m_operation_type;
    CURL* m_curl;

public:
    Api(const std::string& t_name, const std::string& t_url, const std::string& t_key);
    ~Api();

    Api(const Api&) = delete;
    Api& operator=(const Api&) = delete;

    Api(Api&&) noexcept;
    Api& operator=(Api&&) noexcept;

    std::string fetch();
};
}

