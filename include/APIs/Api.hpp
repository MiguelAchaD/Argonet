#pragma once

#include <curl/curl.h>
#include <string>
#include <vector>

namespace proxyServer {
class Api {
private:
    enum class OperationType { GET, POST, PUT };
    std::string m_url;
    std::vector<std::string> m_keys;
    OperationType m_operation_type;
    CURL* m_curl;

    std::string fetch();
    std::string fetchUsingKey();std::string getRandomKey();

public:
    Api(const std::string& t_url);
    ~Api();

    void addKey(std::string t_key);

    Api(const Api&) = delete;
    Api& operator=(const Api&) = delete;

    Api(Api&&) noexcept;
    Api& operator=(Api&&) noexcept;

    virtual std::string callApi();
};
}

