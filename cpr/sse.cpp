#include "cpr/sse.h"

#include <charconv>
#include <cstddef>
#include <functional>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

namespace cpr {

bool ServerSentEventParser::parse(std::string_view data, const std::function<bool(ServerSentEvent&&)>& callback) {
    // Append incoming data to buffer
    buffer_.append(data);

    // Process complete lines
    size_t pos = 0;
    while ((pos = buffer_.find('\n')) != std::string::npos) {
        std::string line = buffer_.substr(0, pos);
        buffer_.erase(0, pos + 1);

        // Remove trailing \r if present (handles both \n and \r\n)
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (!processLine(line, callback)) {
            return false;
        }
    }

    return true;
}

void ServerSentEventParser::reset() {
    buffer_.clear();
    current_event_ = ServerSentEvent();
}

bool ServerSentEventParser::processLine(const std::string& line, const std::function<bool(ServerSentEvent&&)>& callback) {
    // Empty line means end of event
    if (line.empty()) {
        return dispatchEvent(callback);
    }

    // Lines starting with ':' are comments, ignore them
    if (line[0] == ':') {
        return true;
    }

    // Find the colon separator
    const size_t colon_pos = line.find(':');

    std::string field;
    std::string value;

    if (colon_pos == std::string::npos) {
        // No colon, entire line is the field name
        field = line;
        value = "";
    } else {
        field = line.substr(0, colon_pos);
        // Skip the colon and optional leading space
        size_t value_start = colon_pos + 1;
        if (value_start < line.size() && line[value_start] == ' ') {
            value_start++;
        }
        value = line.substr(value_start);
    }

    // Process the field
    if (field == "event") {
        current_event_.event = value;
    } else if (field == "data") {
        // Multiple data fields are concatenated with newlines
        if (!current_event_.data.empty()) {
            current_event_.data += '\n';
        }
        current_event_.data += value;
    } else if (field == "id") {
        // Only set id if the value doesn't contain null character
        if (value.find('\0') == std::string::npos) {
            current_event_.id = value;
        }
    } else if (field == "retry") {
        // Parse retry value as integer
        size_t retry_value = 0;
        const std::string_view sv(value);
        const char* begin = sv.data();
        const char* end = begin + sv.size(); // NOLINT (cppcoreguidelines-pro-bounds-pointer-arithmetic) Required here since Windows and Clang/GCC have different std::string_view iterator implementations
        auto [ptr, ec] = std::from_chars(begin, end, retry_value);
        if (ec == std::errc()) {
            current_event_.retry = retry_value;
        }
    }
    // Unknown fields are ignored per spec

    return true;
}

bool ServerSentEventParser::dispatchEvent(const std::function<bool(ServerSentEvent&&)>& callback) {
    // Don't dispatch if data is empty
    if (current_event_.data.empty()) {
        current_event_ = ServerSentEvent();
        return true;
    }

    // Invoke callback with the current event
    const bool continue_parsing = callback(std::move(current_event_));

    // Reset for next event (but keep event type as "message")
    current_event_ = ServerSentEvent();

    return continue_parsing;
}

bool ServerSentEventCallback::handleData(std::string_view data) {
    return parser_.parse(data, [this](ServerSentEvent&& event) { return (*this)(std::move(event)); });
}

} // namespace cpr
