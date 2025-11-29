#ifndef CPR_SSE_H
#define CPR_SSE_H

#include <cstdint>
#include <functional>
#include <utility>
#include <optional>
#include <string>
#include <string_view>

namespace cpr {

/**
 * Represents a Server-Sent Event (SSE) as defined in the HTML5 specification.
 * https://html.spec.whatwg.org/multipage/server-sent-events.html
 */
struct ServerSentEvent {
    /**
     * The event ID. Can be used to track the last received event and resume from there.
     */
    std::optional<std::string> id;

    /**
     * The event type. If not specified, defaults to "message".
     */
    std::string event{"message"};

    /**
     * The event data. Multiple data fields are concatenated with newlines.
     */
    std::string data;

    /**
     * The retry time in milliseconds. Used to set the reconnection time.
     */
    std::optional<size_t> retry;

    ServerSentEvent() = default;
};

/**
 * Parser for Server-Sent Events (SSE) streams.
 * This parser handles incoming SSE data according to the HTML5 specification.
 */
class ServerSentEventParser {
  public:
    ServerSentEventParser() = default;

    /**
     * Parse incoming SSE data and invoke the callback for each complete event.
     * @param data The incoming data chunk
     * @param callback The callback to invoke for each parsed event
     * @return true to continue receiving data, false to abort
     */
    bool parse(std::string_view data, const std::function<bool(ServerSentEvent&&)>& callback);

    /**
     * Reset the parser state.
     */
    void reset();

  private:
    std::string buffer_;
    ServerSentEvent current_event_;

    bool processLine(const std::string& line, const std::function<bool(ServerSentEvent&&)>& callback);
    bool dispatchEvent(const std::function<bool(ServerSentEvent&&)>& callback);
};

/**
 * Callback for handling Server-Sent Events.
 * The callback receives each parsed SSE event and can return false to abort the connection.
 */
class ServerSentEventCallback {
  public:
    ServerSentEventCallback() = default;
    // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
    ServerSentEventCallback(std::function<bool(ServerSentEvent&& event, intptr_t userdata)> p_callback, intptr_t p_userdata = 0) : userdata(p_userdata), callback(std::move(p_callback)) {}

    bool operator()(ServerSentEvent&& event) const {
        if (!callback) {
            return true;
        }
        return callback(std::move(event), userdata);
    }

    /**
     * Internal function used to handle raw data chunks and parse them into SSE events.
     * This is called by the underlying write callback mechanism.
     */
    bool handleData(std::string_view data);

    intptr_t userdata{};
    std::function<bool(ServerSentEvent&& event, intptr_t userdata)> callback;

  private:
    ServerSentEventParser parser_;
};

} // namespace cpr

#endif
