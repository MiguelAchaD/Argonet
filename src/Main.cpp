#include "Logger.hpp"
#include "Server.hpp"
#include "Command.hpp"
#include "WebServer.hpp"

#include <iostream>
#include <string>
#include <cstdlib>
#include <stdexcept>
#include <csignal>
#include <unistd.h>
#include <thread>
#include <future>

int main(int argc, char *argv[]) {
    std::cout << "PID del programa: " << getpid() << "\n";
    signal(SIGUSR1, proxyServer::Command::setupConfigurationFile);

    bool t_log_to_console = false;
    unsigned short proxy_port = 8080;
    unsigned short web_port = 8081;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--verbose" || arg == "-v") {
            t_log_to_console = true;
        
        } else if (arg == "--port" || arg == "-p") {
            if (i + 1 < argc) {
                try {
                    proxy_port = std::stoi(argv[++i]);
                    if (proxy_port <= 0 || proxy_port >= 65534) {
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
        
        } else if (arg == "--web-port" || arg == "-w") {
            if (i + 1 < argc) {
                try {
                    web_port = std::stoi(argv[++i]);
                    if (web_port <= 0 || web_port >= 65534) {
                        proxyServer::Logger::log("Web port must be between 1 and 65535", proxyServer::Logger::LogType::ERROR);
                        return 1;
                    }
                } catch (const std::invalid_argument& e) {
                    proxyServer::Logger::log("Invalid web port number", proxyServer::Logger::LogType::ERROR);
                    return 1;
                } catch (const std::out_of_range& e) {
                    proxyServer::Logger::log("Web port number out of range", proxyServer::Logger::LogType::ERROR);
                    return 1;
                }
            } else {
                proxyServer::Logger::log("--web-port requires a value", proxyServer::Logger::LogType::ERROR);
                return 1;
            }
        
        } else {
            proxyServer::Logger::log("Unknown argument: " + arg, proxyServer::Logger::LogType::ERROR);
            return 1;
        }
    }

    proxyServer::Logger::initialise(t_log_to_console);

    // Initialize and start the web server asynchronously
    proxyServer::web::WebServer web_server(web_port);
    std::future<void> web_server_future = std::async(std::launch::async, [&web_server]() {
        web_server.start();
    });

    // Give the web server a moment to initialize
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    // Initialize and start the proxy server
    proxyServer::Server proxy_server(proxy_port);
    proxy_server.start();

    // Keep the main thread alive
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
