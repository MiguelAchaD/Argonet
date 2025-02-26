#include "Logger.hpp"
#include "Command.hpp"

#include <sys/stat.h>

bool proxyServer::Logger::m_log_to_console = false;
std::ofstream proxyServer::Logger::m_log_file;
std::mutex proxyServer::Logger::m_mtx;

void proxyServer::Logger::initialise(bool t_log_to_console_flag) {
    m_log_to_console = t_log_to_console_flag;

    std::string m_log_file_path = "log" + Command::getCurrentTimestamp();
    std::string dynamicLogFilePath = Command::getCurrentDir() + "/logs/" + m_log_file_path; 
        
    struct stat info;
    if (stat(dynamicLogFilePath.c_str(), &info) != 0 || !(info.st_mode & S_IFDIR)) {
        mkdir((dynamicLogFilePath + "/logs").c_str(), 0777);
    }

    if (!m_log_file_path.empty()) {
        m_log_file.open(dynamicLogFilePath, std::ios::app);
    }
}

void proxyServer::Logger::log(const std::string& message, LogType type) {
    std::lock_guard<std::mutex> lock(m_mtx);

    std::string colorCode;
    std::string LogTypeStr;

    switch (type) {
        case LogType::LOG:
            colorCode = "\033[0m";
            LogTypeStr = "LOG";
            break;
        case LogType::WARNING:
            colorCode = "\033[33m";
            LogTypeStr = "WARNING";
            break;
        case LogType::ERROR:
            colorCode = "\033[31m";
            LogTypeStr = "ERROR";
            break;
        case LogType::SUCCESS:
            colorCode = "\033[32m";
            LogTypeStr = "SUCCESS";
            break;
    }

    std::string formattedMessage = "[ " + LogTypeStr + " ] " + message;

    if (m_log_to_console) {
        std::cout << colorCode << formattedMessage << "\033[0m" << std::endl;
    }

    if (m_log_file.is_open()) {
        m_log_file << formattedMessage << std::endl;
    }
}

