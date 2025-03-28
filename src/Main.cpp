#include "Logger.hpp"
#include "Server.hpp"
#include "Command.hpp"

#include <iostream>
#include <string>
#include <cstdlib>
#include <stdexcept>
#include <csignal>
#include <unistd.h>

int main(int argc, char *argv[]) {
    std::cout << "PID del programa: " << getpid() << "\n";
    signal(SIGUSR1, proxyServer::Command::setupConfigurationFile);

    bool t_log_to_console = false;
    unsigned short int port = 8080;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--verbose" || arg == "-v") {
            t_log_to_console = true;
        
        } else if (arg == "--port" || arg == "-p") {
            if (i + 1 < argc) {
                try {
                    port = std::stoi(argv[++i]);
                    if (port <= 0 || port >= 65534) {
                        proxyServer::Logger::log("Port must be between 1 and 65535", proxyServer::Logger::LogType::ERROR);
                        return 1;
                    }
                } catch (const std::invalid_argument& e) {
                    proxyServer::Logger::log("Invalid port number", proxyServer::Logger::LogType::ERROR);
                    return 1;
                } catch (const std::out_of_range& e) {
                    proxyServer::Logger::log("Port number out of range", proxyServer::Logger::LogType::ERROR);
                    return 1;
                }
            } else {
                proxyServer::Logger::log("--port requires a value", proxyServer::Logger::LogType::ERROR);
                return 1;
            }
        
        } else {
            proxyServer::Logger::log("Unknown argument: " + arg, proxyServer::Logger::LogType::ERROR);
            return 1;
        }
    }

    proxyServer::Logger::initialise(t_log_to_console);

    proxyServer::Server server = proxyServer::Server(port);
    server.start();
    return 0;
}
