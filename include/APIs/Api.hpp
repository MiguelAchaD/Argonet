#pragma once

#include <curl/curl.h>
#include <string>
#include <vector>
#include <chrono>
#include <deque>
#include <mutex>
#include <memory>

namespace proxyServer {

class Api {
protected:
    enum class OperationType { GET, POST, PUT };
    std::string m_url;
    std::vector<std::string> m_keys;
    OperationType m_operation_type;
    CURL* m_curl;
    int m_requests_per_minute;
    std::deque<std::chrono::system_clock::time_point> m_request_times;
    std::mutex m_mutex;
    size_t m_current_key_index;

    std::string fetch();
    std::string fetchUsingKey();
    std::string getNextKey();
    void waitForRateLimit();
    void addRequestTime();

public:
    Api(const std::string& t_url, int requests_per_minute = 4);
    virtual ~Api();

    void addKey(std::string t_key);
    void setRequestsPerMinute(int requests_per_minute);

    Api(const Api&) = delete;
    Api& operator=(const Api&) = delete;

    Api(Api&&) noexcept;
    Api& operator=(Api&&) noexcept;

    virtual std::string callApi() = 0;
};

}

