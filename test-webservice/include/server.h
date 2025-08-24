#pragma once

#include "config/config.h"
#include <memory>
#include <string>
#include <thread>

namespace test-webservice {

class Server {
public:
    explicit Server(const Config& config);
    ~Server();

    void start();
    void stop();
    bool isRunning() const;

private:
    void setupRoutes();
    void handleRequest(const std::string& method, const std::string& path);

    Config config_;
    bool running_;
    std::unique_ptr<std::thread> serverThread_;
};

} // namespace test-webservice
