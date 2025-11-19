#include <gtest/gtest.h>

#include <chrono>
#include <string>
#include <thread>
#include <vector>

#include "cpr/cpr.h"
#include "cpr/sse.h"
#include "httpServer.hpp"

using namespace cpr;

static HttpServer* server = new HttpServer();

TEST(SSETests, SSEParserBasicTest) {
    ServerSentEventParser parser;
    std::vector<ServerSentEvent> events;

    std::string sse_data = "data: Hello World\n\n";

    parser.parse(sse_data, [&events](ServerSentEvent&& event) {
        events.push_back(std::move(event));
        return true;
    });

    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].data, "Hello World");
    EXPECT_EQ(events[0].event, "message");
    EXPECT_FALSE(events[0].id.has_value());
    EXPECT_FALSE(events[0].retry.has_value());
}

TEST(SSETests, SSEParserMultilineDataTest) {
    ServerSentEventParser parser;
    std::vector<ServerSentEvent> events;

    std::string sse_data = "data: First line\ndata: Second line\n\n";

    parser.parse(sse_data, [&events](ServerSentEvent&& event) {
        events.push_back(std::move(event));
        return true;
    });

    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].data, "First line\nSecond line");
    EXPECT_EQ(events[0].event, "message");
}

TEST(SSETests, SSEParserWithEventTypeTest) {
    ServerSentEventParser parser;
    std::vector<ServerSentEvent> events;

    std::string sse_data = "event: custom\ndata: Custom event data\n\n";

    parser.parse(sse_data, [&events](ServerSentEvent&& event) {
        events.push_back(std::move(event));
        return true;
    });

    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].data, "Custom event data");
    EXPECT_EQ(events[0].event, "custom");
}

TEST(SSETests, SSEParserWithIdTest) {
    ServerSentEventParser parser;
    std::vector<ServerSentEvent> events;

    std::string sse_data = "id: 123\ndata: Event with ID\n\n";

    parser.parse(sse_data, [&events](ServerSentEvent&& event) {
        events.push_back(std::move(event));
        return true;
    });

    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].data, "Event with ID");
    EXPECT_EQ(events[0].event, "message");
    ASSERT_TRUE(events[0].id.has_value());
    EXPECT_EQ(events[0].id.value(), "123");
}

TEST(SSETests, SSEParserWithRetryTest) {
    ServerSentEventParser parser;
    std::vector<ServerSentEvent> events;

    std::string sse_data = "retry: 5000\ndata: Event with retry\n\n";

    parser.parse(sse_data, [&events](ServerSentEvent&& event) {
        events.push_back(std::move(event));
        return true;
    });

    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].data, "Event with retry");
    ASSERT_TRUE(events[0].retry.has_value());
    EXPECT_EQ(events[0].retry.value(), 5000);
}

TEST(SSETests, SSEParserCompleteEventTest) {
    ServerSentEventParser parser;
    std::vector<ServerSentEvent> events;

    std::string sse_data =
            "id: 42\n"
            "event: update\n"
            "retry: 3000\n"
            "data: Complete event data\n"
            "\n";

    parser.parse(sse_data, [&events](ServerSentEvent&& event) {
        events.push_back(std::move(event));
        return true;
    });

    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].data, "Complete event data");
    EXPECT_EQ(events[0].event, "update");
    ASSERT_TRUE(events[0].id.has_value());
    EXPECT_EQ(events[0].id.value(), "42");
    ASSERT_TRUE(events[0].retry.has_value());
    EXPECT_EQ(events[0].retry.value(), 3000);
}

TEST(SSETests, SSEParserMultipleEventsTest) {
    ServerSentEventParser parser;
    std::vector<ServerSentEvent> events;

    std::string sse_data =
            "data: First event\n"
            "\n"
            "data: Second event\n"
            "\n";

    parser.parse(sse_data, [&events](ServerSentEvent&& event) {
        events.push_back(std::move(event));
        return true;
    });

    ASSERT_EQ(events.size(), 2);
    EXPECT_EQ(events[0].data, "First event");
    EXPECT_EQ(events[1].data, "Second event");
}

TEST(SSETests, SSEParserIgnoreCommentsTest) {
    ServerSentEventParser parser;
    std::vector<ServerSentEvent> events;

    std::string sse_data =
            ": This is a comment\n"
            "data: Event data\n"
            ": Another comment\n"
            "\n";

    parser.parse(sse_data, [&events](ServerSentEvent&& event) {
        events.push_back(std::move(event));
        return true;
    });

    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].data, "Event data");
}

TEST(SSETests, SSEParserIgnoreEmptyDataTest) {
    ServerSentEventParser parser;
    std::vector<ServerSentEvent> events;

    std::string sse_data =
            "event: test\n"
            "\n"; // Event with no data should be ignored

    parser.parse(sse_data, [&events](ServerSentEvent&& event) {
        events.push_back(std::move(event));
        return true;
    });

    ASSERT_EQ(events.size(), 0);
}

TEST(SSETests, SSEParserChunkedDataTest) {
    ServerSentEventParser parser;
    std::vector<ServerSentEvent> events;

    // Simulate data arriving in chunks
    parser.parse("data: Partial", [&events](ServerSentEvent&& event) {
        events.push_back(std::move(event));
        return true;
    });

    EXPECT_EQ(events.size(), 0); // No complete event yet

    parser.parse(" event\n\n", [&events](ServerSentEvent&& event) {
        events.push_back(std::move(event));
        return true;
    });

    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].data, "Partial event");
}

TEST(SSETests, SSEParserCRLFTest) {
    ServerSentEventParser parser;
    std::vector<ServerSentEvent> events;

    std::string sse_data = "data: Windows line endings\r\n\r\n";

    parser.parse(sse_data, [&events](ServerSentEvent&& event) {
        events.push_back(std::move(event));
        return true;
    });

    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].data, "Windows line endings");
}

TEST(SSETests, SSEParserFieldWithoutColonTest) {
    ServerSentEventParser parser;
    std::vector<ServerSentEvent> events;

    std::string sse_data =
            "data: Some actual data\n"
            "data\n" // Field without colon should have empty value, appended with newline
            "\n";

    parser.parse(sse_data, [&events](ServerSentEvent&& event) {
        events.push_back(std::move(event));
        return true;
    });

    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].data, "Some actual data\n");
}

TEST(SSETests, SSECallbackTest) {
    ServerSentEventCallback callback(
            [](ServerSentEvent&& /*event*/, intptr_t userdata) {
                int* count = reinterpret_cast<int*>(userdata);
                (*count)++;
                return true;
            },
            0);

    EXPECT_TRUE(callback.callback);
}

TEST(SSETests, SSECallbackHandleDataTest) {
    std::vector<ServerSentEvent> events;
    ServerSentEventCallback callback(
            [&events](ServerSentEvent&& event, intptr_t /*userdata*/) {
                events.push_back(std::move(event));
                return true;
            },
            0);

    std::string sse_data = "data: Test event\n\n";
    callback.handleData(sse_data);

    ASSERT_EQ(events.size(), 1);
    EXPECT_EQ(events[0].data, "Test event");
}

TEST(SSETests, SSECallbackAbortTest) {
    int event_count = 0;
    ServerSentEventCallback callback(
            [&event_count](ServerSentEvent&& /*event*/, intptr_t /*userdata*/) {
                event_count++;
                // Abort after second event
                return event_count < 2;
            },
            0);

    std::string sse_data =
            "data: First\n\n"
            "data: Second\n\n"
            "data: Third\n\n";

    // First two events should be processed, third should cause abort
    callback.handleData(sse_data);

    // The callback is called twice (returns true the first time, false the second)
    // The parser stops after the callback returns false
    EXPECT_EQ(event_count, 2);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(server);
    return RUN_ALL_TESTS();
}
