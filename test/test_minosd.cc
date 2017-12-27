#include <string>
#include <gtest/gtest.h>
#include "minos.grpc.pb.h"
#include "server.h"

using namespace minos;

namespace {

TEST(Server, GetPasswdAll) {
    Server server("localhost:0");
    auto channel = grpc::CreateChannel("127.0.0.1:" + std::to_string(server.port), grpc::InsecureChannelCredentials());
    auto stub = Minos::NewStub(channel);
    grpc::ClientContext context;
    PasswdList reply;
    auto status = stub->GetPasswdAll(&context, Empty(), &reply);
    EXPECT_TRUE(status.ok()) << status.error_message();
    server.grpc_server->Shutdown();
}

TEST(Server, GetPasswdByName) {
    Server server("localhost:0");
    auto channel = grpc::CreateChannel("127.0.0.1:" + std::to_string(server.port), grpc::InsecureChannelCredentials());
    auto stub = Minos::NewStub(channel);
    grpc::ClientContext context;
    ByName request;
    request.set_name("afg");
    Passwd reply;
    auto status = stub->GetPasswdByName(&context, request, &reply);
    EXPECT_TRUE(status.ok()) << status.error_message();
    EXPECT_EQ("afg", reply.pw_name());
    server.grpc_server->Shutdown();
}

TEST(Server, GetPasswdByUid) {
    Server server("localhost:0");
    auto channel = grpc::CreateChannel("127.0.0.1:" + std::to_string(server.port), grpc::InsecureChannelCredentials());
    auto stub = Minos::NewStub(channel);
    grpc::ClientContext context;
    ByID request;
    request.set_id(1000);
    Passwd reply;
    auto status = stub->GetPasswdByUid(&context, request, &reply);
    EXPECT_TRUE(status.ok()) << status.error_message();
    EXPECT_EQ(1000, reply.pw_uid());
    EXPECT_EQ("afg", reply.pw_name());
    server.grpc_server->Shutdown();
}

}
