#include "Api.hpp"

#include <iostream>
#include <stdexcept>
#include <utility>
#include <thread>
#include <chrono>

namespace proxyServer {

Api::Api(const std::string& t_url, int requests_per_minute)
    : m_url(t_url), m_curl(curl_easy_init()), 
      m_requests_per_minute(requests_per_minute),
      m_current_key_index(0) {
    if (!m_curl) {
        throw std::runtime_error("Error al inicializar cURL");
    }
}

Api::~Api() {
    if (m_curl) {
        curl_easy_cleanup(m_curl);
    }
}

void Api::addKey(std::string t_key) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_keys.push_back(t_key);
}

void Api::setRequestsPerMinute(int requests_per_minute) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_requests_per_minute = requests_per_minute;
}

Api::Api(Api&& other) noexcept
    : m_url(std::move(other.m_url)),
      m_keys(std::move(other.m_keys)),
      m_curl(other.m_curl),
      m_requests_per_minute(other.m_requests_per_minute),
      m_current_key_index(other.m_current_key_index) {
    other.m_curl = nullptr;
}

Api& Api::operator=(Api&& other) noexcept {
    if (this != &other) {
        m_url = std::move(other.m_url);
        m_keys = std::move(other.m_keys);
        std::swap(m_curl, other.m_curl);
        m_requests_per_minute = other.m_requests_per_minute;
        m_current_key_index = other.m_current_key_index;
    }
    return *this;
}

size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

void Api::waitForRateLimit() {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    auto now = std::chrono::system_clock::now();
    auto one_minute_ago = now - std::chrono::minutes(1);
    
    // Remove old requests
    while (!m_request_times.empty() && m_request_times.front() < one_minute_ago) {
        m_request_times.pop_front();
    }
    
    // If we've hit the rate limit, wait
    if (m_request_times.size() >= m_requests_per_minute) {
        auto wait_time = m_request_times.front() + std::chrono::minutes(1) - now;
        std::this_thread::sleep_for(wait_time);
    }
}

void Api::addRequestTime() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_request_times.push_back(std::chrono::system_clock::now());
}

std::string Api::getNextKey() {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_keys.empty()) {
        throw std::runtime_error("No API keys available");
    }
    
    std::string key = m_keys[m_current_key_index];
    m_current_key_index = (m_current_key_index + 1) % m_keys.size();
    return key;
}

std::string Api::fetchUsingKey() {
    waitForRateLimit();
    
    std::string key = getNextKey();
    
    std::string response_string;
    curl_easy_setopt(m_curl, CURLOPT_URL, m_url.c_str());
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &response_string);

    // Set headers for VirusTotal API v3
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "accept: application/json");
    std::string api_key_header = "x-apikey: " + key;
    headers = curl_slist_append(headers, api_key_header.c_str());
    curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, headers);

    switch (m_operation_type) {
        case Api::OperationType::POST:
            curl_easy_setopt(m_curl, CURLOPT_POST, 1L);
            break;
        case Api::OperationType::PUT:
            curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, "PUT");
            break;
        case Api::OperationType::GET:
        default:
            break;
    }

    CURLcode res = curl_easy_perform(m_curl);
    curl_slist_free_all(headers);

    if (res != CURLE_OK) {
        throw std::runtime_error("Error en la solicitud cURL: " + std::string(curl_easy_strerror(res)));
    }

    addRequestTime();
    return response_string;
}

std::string Api::fetch() {
    return fetchUsingKey();
}

std::string Api::callApi() {
    return "";
}

}

