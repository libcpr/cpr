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

static void EventHandler(mg_connection* conn, int event, void* event_data, void* context) {
    switch (event) {
        case MG_EV_READ:
        case MG_EV_WRITE:
            /** Do nothing. Just for housekeeping. **/
            break;
        case MG_EV_POLL:
            /** Do nothing. Just for housekeeping. **/
            break;
        case MG_EV_CLOSE:
            /** Do nothing. Just for housekeeping. **/
            break;
        case MG_EV_ACCEPT:
            /* Initialize HTTPS connection if Server is an HTTPS Server */
            static_cast<AbstractServer*>(context)->acceptConnection(conn);
            break;
        case MG_EV_CONNECT:
            /** Do nothing. Just for housekeeping. **/
            break;

        case MG_EV_HTTP_CHUNK: {
            /** Do nothing. Just for housekeeping. **/
        } break;

        case MG_EV_HTTP_MSG: {
            AbstractServer* server = static_cast<AbstractServer*>(context);
            server->OnRequest(conn, static_cast<mg_http_message*>(event_data));
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
        mg_mgr_poll(&mgr, 100);
    }

    // Shutdown and cleanup:
    timer_args.clear();
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

    std::vector<int> T(256, -1);
    for (size_t i = 0; i < 64; i++)
        T[base64_chars[i]] = static_cast<int>(i);

    int val = 0;
    int valb = -8;
    for (unsigned char c : in) {
        if (T[c] == -1) {
            break;
        }
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

// Sends error similar like in mongoose 6 method mg_http_send_error
// https://github.com/cesanta/mongoose/blob/6.18/mongoose.c#L7081-L7089
void AbstractServer::SendError(mg_connection* conn, int code, std::string& reason) {
    std::string headers{"Content-Type: text/plain\r\nConnection: close\r\n"};
    mg_http_reply(conn, code, headers.c_str(), reason.c_str());
}

// Checks whether a pointer to a connection is still managed by a mg_mgr.
// This check tells whether it is still possible to send a message via the given connection
// Note that it is still possible that the pointer of an old connection object may be reused by mongoose.
// In this case, the active connection might refer to a different connection than the one the caller refers to
bool AbstractServer::IsConnectionActive(mg_mgr* mgr, mg_connection* conn) {
    mg_connection* c{mgr->conns};
    while (c) {
        if (c == conn) {
            return true;
        }
        c = c->next;
    }
    return false;
}

uint16_t AbstractServer::GetRemotePort(const mg_connection* conn) {
    return (conn->rem.port >> 8) | (conn->rem.port << 8);
}

uint16_t AbstractServer::GetLocalPort(const mg_connection* conn) {
    return (conn->loc.port >> 8) | (conn->loc.port << 8);
}

} // namespace cpr
