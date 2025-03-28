#include "Accepter.hpp"
#include "Logger.hpp"

#include <iostream>
#include <unistd.h> 
#include <regex>

proxyServer::Accepter::Accepter() {

}

proxyServer::Accepter::~Accepter() {

}

proxyServer::petitionPacket proxyServer::Accepter::parseContents(std::string t_result) {
    proxyServer::petitionPacket result;

    std::regex pattern(R"((?=\s*)(Host|User-Agent|Accept|Accept-Language|Accept-Encoding|Connection|Pragma|Cache)(?=\s*):(\s*)(.*?)(?=\r?\n|$))");
    std::smatch matches;

    auto begin = t_result.cbegin();
    auto end = t_result.cend();
    while (std::regex_search(begin, end, matches, pattern)) {
        std::string key = matches[1];
        std::string value = matches[3];

        if (key == "Host") {
            result.host = value;
        } else if (key == "User-Agent") {
            result.user_agent = value;
        } else if (key == "Accept") {
            result.accept = value;
        } else if (key == "Accept-Language") {
            result.accept_language = value;
        } else if (key == "Accept-Encoding") {
            result.accept_encoding = value;
        } else if (key == "Connection") {
            result.connection_type = value;
        } else if (key == "Pragma") {
            result.pragma = value;
        } else if (key == "Cache") {
            result.cache = value;
        }

        begin = matches[0].second;
    }

    return result;
}

std::string proxyServer::Accepter::readContents(int t_client_socket) {
    char buffer[1024] = {0};
    ssize_t bytes_received = read(t_client_socket, buffer, 1024);
    if (bytes_received < 0) {
        proxyServer::Logger::log("Failed to read data from client", proxyServer::Logger::LogType::ERROR);
        return "";
    }
    return std::string(buffer);
}

proxyServer::petitionPacket proxyServer::Accepter::examineContents(int t_client_socket) {
    std::string result = readContents(t_client_socket);
    proxyServer::petitionPacket contents = parseContents(result);
    contents.client_socket = t_client_socket;
    
    return contents;
}
