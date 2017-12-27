#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>

#include "minos.grpc.pb.h"

using namespace minos;

int main() {
    auto channel = grpc::CreateChannel("10.12.25.1:2047", grpc::InsecureChannelCredentials());
    auto stub = minos::Minos::NewStub(channel);
    minos::PasswdList reply;
    grpc::ClientContext context;
    grpc::Status status = stub->GetPasswdAll(&context, Empty(), &reply);
    if (status.ok()) {
        for (const auto& entry: reply.entries()) {
            std::cout << entry.pw_name() << std::endl;
        }
    } else {
        std::cout << status.error_code() << ":" << status.error_message() << std::endl;
    }
}
