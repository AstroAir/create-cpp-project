#include "server.h"
#include "config/config.h"
#include "utils/logger.h"
#include <iostream>
#include <signal.h>

// Global server instance for signal handling
std::unique_ptr<test-webservice::Server> g_server;

void signalHandler(int signal) {
    if (g_server) {
        std::cout << "\nShutting down server gracefully...\n";
        g_server->stop();
    }
    exit(signal);
}

int main(int argc, char* argv[]) {
    try {
        // Initialize logger
        test-webservice::Logger::initialize();

        // Load configuration
        test-webservice::Config config;
        if (argc > 1) {
            config.loadFromFile(argv[1]);
        } else {
            config.loadDefaults();
        }

        // Setup signal handlers
        signal(SIGINT, signalHandler);
        signal(SIGTERM, signalHandler);

        // Create and start server
        g_server = std::make_unique<test-webservice::Server>(config);

        std::cout << "Starting test-webservice server on port " << config.getPort() << "...\n";
        g_server->start();

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}
