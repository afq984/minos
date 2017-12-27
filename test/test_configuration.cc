#include <cstdlib>

#include <gtest/gtest.h>

#include "configuration.h"

namespace {

TEST(Configuration, Parse) {
    const char* test_config = getenv("TEST_CONFIGURATION_FILE");
    ASSERT_NE(test_config, nullptr) << "TEST_CONFIGURATION_FILE environment variable not specified";
    ASSERT_EQ("", minos::config::get_server_address());
    EXPECT_EQ(0, minos::config::parse(test_config));
    EXPECT_EQ("www.example.com:2047", minos::config::get_server_address());
}

}
