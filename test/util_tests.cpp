#include <gtest/gtest.h>

#include <string>

#include <cpr/cprtypes.h>
#include <cpr/util.h>

using namespace cpr;

TEST(UtilParseCookiesTests, BasicParseTest) {
    Cookies expectedCookies{{Cookie("status", "on", "127.0.0.1", false, "/", false, std::chrono::system_clock::from_time_t(1656908640)), Cookie("name", "debug", "127.0.0.1", false, "/", false, std::chrono::system_clock::from_time_t(0))}};
    curl_slist* raw_cookies = new curl_slist{
            (char*) "127.0.0.1\tFALSE\t/\tFALSE\t1656908640\tstatus\ton",
            new curl_slist{
                    (char*) "127.0.0.1\tFALSE\t/\tFALSE\t0\tname\tdebug",
                    nullptr,
            },
    };
    Cookies cookies = util::parseCookies(raw_cookies);
    for (auto cookie = cookies.begin(), expectedCookie = expectedCookies.begin(); cookie != cookies.end() && expectedCookie != expectedCookies.end(); cookie++, expectedCookie++) {
        EXPECT_EQ(expectedCookie->GetName(), cookie->GetName());
        EXPECT_EQ(expectedCookie->GetValue(), cookie->GetValue());
        EXPECT_EQ(expectedCookie->GetDomain(), cookie->GetDomain());
        EXPECT_EQ(expectedCookie->IsIncludingSubdomains(), cookie->IsIncludingSubdomains());
        EXPECT_EQ(expectedCookie->GetPath(), cookie->GetPath());
        EXPECT_EQ(expectedCookie->IsHttpsOnly(), cookie->IsHttpsOnly());
        EXPECT_EQ(expectedCookie->GetExpires(), cookie->GetExpires());
    }
    delete raw_cookies->next;
    delete raw_cookies;
}

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

TEST(UtilSecureStringClearTests, EmptyStringTest) {
    std::string input;
    util::secureStringClear(input);
    EXPECT_TRUE(input.empty());
}

TEST(UtilSecureStringClearTests, NotEmptyStringTest) {
    std::string input = "Hello World!";
    util::secureStringClear(input);
    EXPECT_TRUE(input.empty());
}

TEST(UtilIsTrueTests, TrueTest) {
    {
        std::string input = "TRUE";
        bool output = util::isTrue(input);
        EXPECT_TRUE(output);
    }
    {
        std::string input = "True";
        bool output = util::isTrue(input);
        EXPECT_TRUE(output);
    }
    {
        std::string input = "true";
        bool output = util::isTrue(input);
        EXPECT_TRUE(output);
    }
}

TEST(UtilIsTrueTests, FalseTest) {
    {
        std::string input = "FALSE";
        bool output = util::isTrue(input);
        EXPECT_FALSE(output);
    }
    {
        std::string input = "False";
        bool output = util::isTrue(input);
        EXPECT_FALSE(output);
    }
    {
        std::string input = "false";
        bool output = util::isTrue(input);
        EXPECT_FALSE(output);
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
