#pragma once

#include <string>
#include <array>
#include <cstring>
#include <string_view>

namespace proxyServer {

class FastString {
private:
    static constexpr size_t SMALL_STRING_SIZE = 32;
    std::array<char, SMALL_STRING_SIZE> m_small;
    std::string m_large;
    bool m_is_small{true};

public:
    FastString() {
        m_small[0] = '\0';
    }

    FastString(const char* str) {
        size_t len = strlen(str);
        if (len < SMALL_STRING_SIZE) {
            m_is_small = true;
            memcpy(m_small.data(), str, len + 1);
        } else {
            m_is_small = false;
            m_large = str;
        }
    }

    FastString(const std::string& str) {
        if (str.size() < SMALL_STRING_SIZE) {
            m_is_small = true;
            memcpy(m_small.data(), str.c_str(), str.size() + 1);
        } else {
            m_is_small = false;
            m_large = str;
        }
    }

    const char* c_str() const {
        return m_is_small ? m_small.data() : m_large.c_str();
    }

    size_t size() const {
        return m_is_small ? strlen(m_small.data()) : m_large.size();
    }

    bool empty() const {
        return m_is_small ? (m_small[0] == '\0') : m_large.empty();
    }

    void clear() {
        if (m_is_small) {
            m_small[0] = '\0';
        } else {
            m_large.clear();
        }
    }

    void assign(const char* str) {
        size_t len = strlen(str);
        if (len < SMALL_STRING_SIZE) {
            m_is_small = true;
            memcpy(m_small.data(), str, len + 1);
        } else {
            m_is_small = false;
            m_large = str;
        }
    }

    void assign(const std::string& str) {
        if (str.size() < SMALL_STRING_SIZE) {
            m_is_small = true;
            memcpy(m_small.data(), str.c_str(), str.size() + 1);
        } else {
            m_is_small = false;
            m_large = str;
        }
    }
};

struct petitionPacket {
    FastString host;
    FastString user_agent;
    FastString accept;
    FastString accept_language;
    FastString accept_encoding;
    FastString connection_type;
    FastString pragma;
    FastString cache;
    unsigned short int client_socket;
    bool isAccepted = false;
    bool isResolved = false;
    bool isForwarder = false;
    FastString response;

    bool isEmpty() const {
        return host.empty();
    }

    std::string toString() const {
        static thread_local std::string result;
        result.clear();
        result.reserve(512);  // Pre-allocate space for common case

        result = "GET / HTTP/1.1\nHost: ";
        result += host.c_str();
        result += "\nUser-Agent: ";
        
        if (user_agent.empty()) {
            result += "Mozilla/5.0";
        } else {
            result += user_agent.c_str();
        }

        result += "\nAccept: ";
        if (accept.empty()) {
            result += "text/html\n";
        } else {
            result += accept.c_str();
            result += "\n";
        }
        
        if (!accept_language.empty()) {
            result += "Accept-Language: ";
            result += accept_language.c_str();
            result += "\n";
        }

        if (!accept_encoding.empty()) {
            result += "Accept-Encoding: ";
            result += accept_encoding.c_str();
            result += "\n";
        }

        if (!connection_type.empty()) {
            result += "Connection: ";
            result += connection_type.c_str();
            result += "\n";
        }

        if (!pragma.empty()) {
            result += "Pragma: ";
            result += pragma.c_str();
            result += "\n";
        }

        if (!cache.empty()) {
            result += "Cache-Control: ";
            result += cache.c_str();
            result += "\n";
        }
        result += "\n";

        return result;
    }

    void clear() {
        host.clear();
        user_agent.clear();
        accept.clear();
        accept_language.clear();
        accept_encoding.clear();
        connection_type.clear();
        pragma.clear();
        cache.clear();
        response.clear();
        isAccepted = false;
        isResolved = false;
        isForwarder = false;
    }
};

} // namespace proxyServer
