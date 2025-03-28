#include "Api.hpp"

#include <iostream>
#include <stdexcept>
#include <utility>

namespace proxyServer {

Api::Api(const std::string& t_name, const std::string& t_url, const std::string& t_key)
    : m_name(t_name), m_url(t_url), m_key(t_key), m_curl(curl_easy_init()) {
    if (!m_curl) {
        throw std::runtime_error("Error al inicializar cURL");
    }
}

Api::~Api() {
    if (m_curl) {
        curl_easy_cleanup(m_curl);
    }
}

Api::Api(Api&& other) noexcept
    : m_name(std::move(other.m_name)),
      m_url(std::move(other.m_url)),
      m_key(std::move(other.m_key)),
      m_curl(other.m_curl) {
    other.m_curl = nullptr;
}

Api& Api::operator=(Api&& other) noexcept {
    if (this != &other) {
        m_name = std::move(other.m_name);
        m_url = std::move(other.m_url);
        m_key = std::move(other.m_key);
        std::swap(m_curl, other.m_curl);
    }
    return *this;
}

size_t writeCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

std::string Api::fetch() {
    if (!m_curl) {
        throw std::runtime_error("cURL no está inicializado");
    }

    std::string response_string;
    curl_easy_setopt(m_curl, CURLOPT_URL, m_url.c_str());
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &response_string);

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
    if (res != CURLE_OK) {
        throw std::runtime_error("Error en la solicitud cURL: " + std::string(curl_easy_strerror(res)));
    }

    return response_string;
}

}

