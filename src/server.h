#include <memory>
#include <string>
#include <grpc++/grpc++.h>
#include "minos.grpc.pb.h"

namespace minos {

struct Server {
    std::unique_ptr<Minos::Service> service;
    int port;
    std::unique_ptr<grpc::Server> grpc_server;
    Server(const std::string& address);
};

} // namespace minos
