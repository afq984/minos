#include <iostream>
#include <memory>

#include <grpc++/grpc++.h>

#include "configuration.h"
#include "server.h"

int main() {
    minos::config::parse(minos::config::get_config_path());
    const std::string& address = minos::config::get_server_address();
    if (address.empty()) {
        std::cerr << "Error: server address is not configured" << std::endl;
        return 1;
    }
    minos::Server server(address);
    std::cout << "Running server at: " << address << std::endl;
    server.grpc_server->Wait();
}
