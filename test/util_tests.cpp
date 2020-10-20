#include <gtest/gtest.h>

#include <string>

#include <cpr/cprtypes.h>
#include <cpr/util.h>

using namespace cpr;

TEST(UtilParseHeaderTests, BasicParseTest) {
    std::string header_string{
            "HTTP/1.1 200 OK\r\n"
            "Server: nginx\r\n"
            "Date: Sun, 05 Mar 2017 00:34:54 GMT\r\n"
            "Content-Type: application/json\r\n"
            "Content-Length: 351\r\n"
            "Connection: keep-alive\r\n"
            "Access-Control-Allow-Origin: *\r\n"
            "Access-Control-Allow-Credentials: true\r\n"
            "\r\n"};
    Header header = util::parseHeader(header_string);
    EXPECT_EQ(std::string{"nginx"}, header["Server"]);
    EXPECT_EQ(std::string{"Sun, 05 Mar 2017 00:34:54 GMT"}, header["Date"]);
    EXPECT_EQ(std::string{"application/json"}, header["Content-Type"]);
    EXPECT_EQ(std::string{"351"}, header["Content-Length"]);
    EXPECT_EQ(std::string{"keep-alive"}, header["Connection"]);
    EXPECT_EQ(std::string{"*"}, header["Access-Control-Allow-Origin"]);
    EXPECT_EQ(std::string{"true"}, header["Access-Control-Allow-Credentials"]);
}

TEST(UtilParseHeaderTests, NewlineTest) {
    std::string header_string{
            "HTTP/1.1 200 OK\r\n"
            "Auth:\n"
            "Access-Control-Allow-Credentials: true\r\n"
            "\r\n"};
    Header header = util::parseHeader(header_string);
    EXPECT_EQ(std::string{""}, header["Server"]);
    EXPECT_EQ(std::string{"true"}, header["Access-Control-Allow-Credentials"]);
}

TEST(UtilParseHeaderTests, SpaceNewlineTest) {
    std::string header_string{
            "HTTP/1.1 200 OK\r\n"
            "Auth: \n"
            "Access-Control-Allow-Credentials: true\r\n"
            "\r\n"};
    Header header = util::parseHeader(header_string);
    EXPECT_EQ(std::string{""}, header["Server"]);
    EXPECT_EQ(std::string{"true"}, header["Access-Control-Allow-Credentials"]);
}

TEST(UtilParseHeaderTests, CarriageReturnNewlineTest) {
    std::string header_string{
            "HTTP/1.1 200 OK\n"
            "Auth:\r\n"
            "Access-Control-Allow-Credentials: true\r\n"
            "\r\n"};
    Header header = util::parseHeader(header_string);
    EXPECT_EQ(std::string{""}, header["Server"]);
    EXPECT_EQ(std::string{"true"}, header["Access-Control-Allow-Credentials"]);
}

TEST(UtilParseHeaderTests, SpaceCarriageReturnNewlineTest) {
    std::string header_string{
            "HTTP/1.1 200 OK\n"
            "Auth: \r\n"
            "Access-Control-Allow-Credentials: true\r\n"
            "\r\n"};
    Header header = util::parseHeader(header_string);
    EXPECT_EQ(std::string{""}, header["Server"]);
    EXPECT_EQ(std::string{"true"}, header["Access-Control-Allow-Credentials"]);
}

TEST(UtilParseHeaderTests, BasicStatusLineTest) {
    std::string header_string{
            "HTTP/1.1 200 OK\r\n"
            "Server: nginx\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"};
    std::string status_line;
    std::string reason;
    Header header = util::parseHeader(header_string, &status_line, &reason);
    EXPECT_EQ(std::string{"HTTP/1.1 200 OK"}, status_line);
    EXPECT_EQ(std::string{"OK"}, reason);
    EXPECT_EQ(std::string{"nginx"}, header["Server"]);
    EXPECT_EQ(std::string{"application/json"}, header["Content-Type"]);
}

TEST(UtilParseHeaderTests, NewlineStatusLineTest) {
    std::string header_string{
            "HTTP/1.1 407 Proxy Authentication Required\n"
            "Server: nginx\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"};
    std::string status_line;
    std::string reason;
    Header header = util::parseHeader(header_string, &status_line, &reason);
    EXPECT_EQ(std::string{"HTTP/1.1 407 Proxy Authentication Required"}, status_line);
    EXPECT_EQ(std::string{"Proxy Authentication Required"}, reason);
    EXPECT_EQ(std::string{"nginx"}, header["Server"]);
    EXPECT_EQ(std::string{"application/json"}, header["Content-Type"]);
}

TEST(UtilParseHeaderTests, NoReasonSpaceTest) {
    std::string header_string{
            "HTTP/1.1 200 \n"
            "Server: nginx\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"};
    std::string status_line;
    std::string reason;
    Header header = util::parseHeader(header_string, &status_line, &reason);
    EXPECT_EQ(std::string{"HTTP/1.1 200"}, status_line);
    EXPECT_EQ(std::string{""}, reason);
    EXPECT_EQ(std::string{"nginx"}, header["Server"]);
    EXPECT_EQ(std::string{"application/json"}, header["Content-Type"]);
}

TEST(UtilParseHeaderTests, NoReasonTest) {
    std::string header_string{
            "HTTP/1.1 200\n"
            "Server: nginx\r\n"
            "Content-Type: application/json\r\n"
            "\r\n"};
    std::string status_line;
    std::string reason;
    Header header = util::parseHeader(header_string, &status_line, &reason);
    EXPECT_EQ(std::string{"HTTP/1.1 200"}, status_line);
    EXPECT_EQ(std::string{""}, reason);
    EXPECT_EQ(std::string{"nginx"}, header["Server"]);
    EXPECT_EQ(std::string{"application/json"}, header["Content-Type"]);
}

TEST(UtilUrlEncodeTests, UnicodeEncoderTest) {
    std::string input = "一二三";
    std::string result = util::urlEncode(input);
    std::string expected = "%E4%B8%80%E4%BA%8C%E4%B8%89";
    EXPECT_EQ(result, expected);
}

TEST(UtilUrlEncodeTests, AsciiEncoderTest) {
    std::string input = "Hello World!";
    std::string result = util::urlEncode(input);
    std::string expected = "Hello%20World%21";
    EXPECT_EQ(result, expected);
}

TEST(UtilUrlDecodeTests, UnicodeDecoderTest) {
    std::string input = "%E4%B8%80%E4%BA%8C%E4%B8%89";
    std::string result = util::urlDecode(input);
    std::string expected = "一二三";
    EXPECT_EQ(result, expected);
}

TEST(UtilUrlDecodeTests, AsciiDecoderTest) {
    std::string input = "Hello%20World%21";
    std::string result = util::urlDecode(input);
    std::string expected = "Hello World!";
    EXPECT_EQ(result, expected);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
