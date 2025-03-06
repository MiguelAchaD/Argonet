#pragma once

#include <string>
#include <fstream>
#include <iostream>
#include <mutex>

namespace proxyServer {
class Logger {
    public:
        enum class LogType {
            LOG,
            WARNING,
            ERROR,
            SUCCESS
        };

        static void initialise(bool t_log_to_console);
        static void log(const std::string& t_message, LogType t_type = LogType::LOG);

    private:
        static bool m_log_to_console;
        static std::ofstream m_log_file;
        static std::mutex m_mtx;
};
}
