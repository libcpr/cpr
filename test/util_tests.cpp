#include <gtest/gtest.h>

#include <string>

#include <cpr/cprtypes.h>
#include <cpr/util.h>

using namespace cpr;

TEST(UtilParseHeaderTests, BasicParseTest) {
    auto header_string = std::string{
        "HTTP/1.1 200 OK\r\n"
        "Server: nginx\r\n"
        "Date: Sun, 05 Mar 2017 00:34:54 GMT\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 351\r\n"
        "Connection: keep-alive\r\n"
        "Access-Control-Allow-Origin: *\r\n"
        "Access-Control-Allow-Credentials: true\r\n"
        "\r\n"};
    auto header = util::parseHeader(header_string);
    EXPECT_EQ(std::string{"nginx"}, header["Server"]);
    EXPECT_EQ(std::string{"Sun, 05 Mar 2017 00:34:54 GMT"}, header["Date"]);
    EXPECT_EQ(std::string{"application/json"}, header["Content-Type"]);
    EXPECT_EQ(std::string{"351"}, header["Content-Length"]);
    EXPECT_EQ(std::string{"keep-alive"}, header["Connection"]);
    EXPECT_EQ(std::string{"*"}, header["Access-Control-Allow-Origin"]);
    EXPECT_EQ(std::string{"true"}, header["Access-Control-Allow-Credentials"]);
}

TEST(UtilParseHeaderTests, NewlineTest) {
    auto header_string = std::string{
        "HTTP/1.1 200 OK\r\n"
        "Auth:\n"
        "Access-Control-Allow-Credentials: true\r\n"
        "\r\n"};
    auto header = util::parseHeader(header_string);
    EXPECT_EQ(std::string{""}, header["Server"]);
    EXPECT_EQ(std::string{"true"}, header["Access-Control-Allow-Credentials"]);
}

TEST(UtilParseHeaderTests, SpaceNewlineTest) {
    auto header_string = std::string{
        "HTTP/1.1 200 OK\r\n"
        "Auth: \n"
        "Access-Control-Allow-Credentials: true\r\n"
        "\r\n"};
    auto header = util::parseHeader(header_string);
    EXPECT_EQ(std::string{""}, header["Server"]);
    EXPECT_EQ(std::string{"true"}, header["Access-Control-Allow-Credentials"]);
}

TEST(UtilParseHeaderTests, CarriageReturnNewlineTest) {
    auto header_string = std::string{
        "HTTP/1.1 200 OK\n"
        "Auth:\r\n"
        "Access-Control-Allow-Credentials: true\r\n"
        "\r\n"};
    auto header = util::parseHeader(header_string);
    EXPECT_EQ(std::string{""}, header["Server"]);
    EXPECT_EQ(std::string{"true"}, header["Access-Control-Allow-Credentials"]);
}

TEST(UtilParseHeaderTests, SpaceCarriageReturnNewlineTest) {
    auto header_string = std::string{
        "HTTP/1.1 200 OK\n"
        "Auth: \r\n"
        "Access-Control-Allow-Credentials: true\r\n"
        "\r\n"};
    auto header = util::parseHeader(header_string);
    EXPECT_EQ(std::string{""}, header["Server"]);
    EXPECT_EQ(std::string{"true"}, header["Access-Control-Allow-Credentials"]);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
