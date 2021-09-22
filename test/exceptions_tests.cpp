#include <gtest/gtest.h>

#include <string>

#include "cpr/exceptions.h"
#include <cpr/cpr.h>

using namespace cpr;

// HTTP code generator
Url url{"https://httpstat.us/"};

TEST(ExceptionsTest, HTTPErrorCode) {
    for (int safe_code = 200; safe_code < 206; safe_code++) {
        Response response = cpr::Get(url + std::to_string(safe_code));
        EXPECT_EQ(safe_code, response.status_code);
        EXPECT_NO_THROW(response.ThrowForStatus());
    }
    for (int client_code = 400; client_code < 420;client_code++) {
        Response response = cpr::Get( url+std::to_string(client_code));
        EXPECT_EQ(client_code, response.status_code);
        try {
            response.ThrowForStatus();
        } catch (Exception::BadResponse& e) {
            EXPECT_EQ(client_code, e.http_code);
        }
    }
    for (int server_code = 500; server_code < 504; server_code++) {
        Response response = cpr::Get(url + std::to_string(server_code));
        EXPECT_EQ(server_code, response.status_code);
        try {
            response.ThrowForStatus();
        } catch(Exception::BadResponse& e) {
            EXPECT_EQ(server_code,e.http_code);
        }
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}