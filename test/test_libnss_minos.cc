#include <nss.h>
#include <pwd.h>
#include <memory>
#include <gtest/gtest.h>
#include <grpc++/grpc++.h>

#include "server.h"
#include "minos.grpc.pb.h"

using namespace minos;

namespace minos {
void set_server_address(const std::string&);
}

extern "C" enum nss_status _nss_minos_getpwnam_r(
    const char* name, struct passwd* pwd, char* buf, size_t buflen, int* errnop
);
extern "C" enum nss_status _nss_minos_getpwuid_r(
    uid_t uid, struct passwd* pwd, char* buf, size_t buflen, int* errnop
);

namespace {

class NssTest: public ::testing::Test {
protected:
    Server server;
    NssTest(): server("127.0.0.1:0") {}
    virtual ~NssTest() {}
    virtual void SetUp() override {
        ASSERT_NE(0, server.port);
        set_server_address("127.0.0.1:" + std::to_string(server.port));
    }
};

TEST_F(NssTest, GetPwNam) {
    char* buf = (char*)malloc(1024);
    struct passwd pwd;
    int err = 0;
    enum nss_status status = _nss_minos_getpwnam_r("afg", &pwd, buf, 1024, &err);
    EXPECT_EQ(NSS_STATUS_SUCCESS, status);
    EXPECT_EQ(0, err);
    EXPECT_STREQ("afg", pwd.pw_name);
}

TEST_F(NssTest, GetPwUid) {
    char* buf = (char*)malloc(1024);
    struct passwd pwd;
    int err = 0;
    enum nss_status status = _nss_minos_getpwuid_r(1000, &pwd, buf, 1024, &err);
    EXPECT_EQ(NSS_STATUS_SUCCESS, status);
    EXPECT_EQ(0, err) << strerror(err);
    EXPECT_STREQ("afg", pwd.pw_name);
    EXPECT_EQ(1000, pwd.pw_uid);
}

}
