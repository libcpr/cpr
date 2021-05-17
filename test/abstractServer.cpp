#include "abstractServer.hpp"

namespace cpr {
void AbstractServer::SetUp() {
    Start();
}

void AbstractServer::TearDown() {
    Stop();
}

void AbstractServer::Start() {
    should_run = true;
    serverThread = std::make_shared<std::thread>(&AbstractServer::Run, this);
    serverThread->detach();
    std::unique_lock<std::mutex> server_lock(server_mutex);
    server_start_cv.wait(server_lock);
}

void AbstractServer::Stop() {
    should_run = false;
    std::unique_lock<std::mutex> server_lock(server_mutex);
    server_stop_cv.wait(server_lock);
}

static void EventHandler(mg_connection* conn, int event, void* event_data) {
    switch (event) {
        case MG_EV_RECV:
        case MG_EV_SEND:
            /** Do nothing. Just for housekeeping. **/
            break;
        case MG_EV_POLL:
            /** Do nothing. Just for housekeeping. **/
            break;
        case MG_EV_CLOSE:
            /** Do nothing. Just for housekeeping. **/
            break;
        case MG_EV_ACCEPT:
            /** Do nothing. Just for housekeeping. **/
            break;
        case MG_EV_CONNECT:
            /** Do nothing. Just for housekeeping. **/
            break;
        case MG_EV_TIMER:
            /** Do nothing. Just for housekeeping. **/
            break;

        case MG_EV_HTTP_CHUNK: {
            /** Do nothing. Just for housekeeping. **/
        } break;

        case MG_EV_HTTP_REQUEST: {
            AbstractServer* server = static_cast<AbstractServer*>(conn->mgr->user_data);
            server->OnRequest(conn, static_cast<http_message*>(event_data));
        } break;

        default:
            break;
    }
}

void AbstractServer::Run() {
    // Setup a new mongoose http server.
    memset(&mgr, 0, sizeof(mg_mgr));
    initServer(&mgr, EventHandler);

    // Notify the main thread that the server is up and runing:
    server_start_cv.notify_all();

    // Main server loop:
    while (should_run) {
        // NOLINTNEXTLINE (cppcoreguidelines-avoid-magic-numbers)
        mg_mgr_poll(&mgr, 1000);
    }

    // Shutdown and cleanup:
    mg_mgr_free(&mgr);

    // Notify the main thread that we have shut down everything:
    server_stop_cv.notify_all();
}

static const std::string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
/**
 * Decodes the given BASE64 string to a normal string.
 * Source: https://gist.github.com/williamdes/308b95ac9ef1ee89ae0143529c361d37
 **/
std::string AbstractServer::Base64Decode(const std::string& in) {
    std::string out;

    // NOLINTNEXTLINE (cppcoreguidelines-avoid-magic-numbers)
    std::vector<int> T(256, -1);
    // NOLINTNEXTLINE (cppcoreguidelines-avoid-magic-numbers)
    for (size_t i = 0; i < 64; i++)
        T[base64_chars[i]] = i;

    int val = 0;
    // NOLINTNEXTLINE (cppcoreguidelines-avoid-magic-numbers)
    int valb = -8;
    for (unsigned char c : in) {
        if (T[c] == -1) {
            break;
        }
        // NOLINTNEXTLINE (cppcoreguidelines-avoid-magic-numbers)
        val = (val << 6) + T[c];
        // NOLINTNEXTLINE (cppcoreguidelines-avoid-magic-numbers)
        valb += 6;
        if (valb >= 0) {
            // NOLINTNEXTLINE (cppcoreguidelines-avoid-magic-numbers)
            out.push_back(char((val >> valb) & 0xFF));
            // NOLINTNEXTLINE (cppcoreguidelines-avoid-magic-numbers)
            valb -= 8;
        }
    }
    return out;
}

} // namespace cpr
