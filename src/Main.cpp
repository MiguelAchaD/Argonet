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

using namespace proxyServer;

void printHelp() {
    std::cout << "Usage: argonet [options]\n"
              << "Options:\n"
              << "  --help, -h     Show this help message\n"
              << "  --verbose, -v  Enable verbose logging to console\n"
              << "\n"
              << "Configuration:\n"
              << "  The server uses configuration.json for settings.\n"
              << "  Ports and other settings can be modified in the configuration file.\n";
}

int main(int argc, char *argv[]) {
    bool t_log_to_console = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            printHelp();
            return 0;
        } else if (arg == "--verbose" || arg == "-v") {
            t_log_to_console = true;
        } else {
            Logger::log("Unknown argument: " + arg, Logger::LogType::ERROR);
            printHelp();
            return 1;
        }
    }

    Logger::initialise(t_log_to_console);

    try {
        // Initialize server
        Server& server = Server::getInstance();
        
        // Initialize web server
        web::WebServer webServer(8081); // Port from configuration
        webServer.setServer(&server);
        
        // Start web server
        webServer.start();
        
        // Start main server
        server.start();
        
        // Wait for server to finish
        server.wait();
        
        // Stop web server
        webServer.stop();
        
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}