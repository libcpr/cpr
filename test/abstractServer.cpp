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
            // Use the connection address as unique identifier instead
            int port = AbstractServer::GetRemotePort(conn);
            server->AddConnection(port);
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

void AbstractServer::AddConnection(int remote_port) {
    unique_connections.insert(remote_port);
}

size_t AbstractServer::GetConnectionCount() {
    return unique_connections.size();
}

void AbstractServer::ResetConnectionCount() {
    unique_connections.clear();
}


std::string AbstractServer::Base64Decode(const std::string& in) {
    static const unsigned char T[256] = []{
        unsigned char t[256];
        std::fill(std::begin(t), std::end(t), 0xFF);
        static const char base64_chars[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";
        for (int i = 0; i < 64; i++)
            t[static_cast<unsigned char>(base64_chars[i])] = i;
        return t;
    }();

    size_t in_len = in.size();
    if (in_len % 4 != 0) return {}; // invalid

    size_t out_len = in_len / 4 * 3;
    if (in_len && in[in_len - 1] == '=') out_len--;
    if (in_len > 1 && in[in_len - 2] == '=') out_len--;

    std::string out(out_len, '\0');
    size_t o = 0;

    for (size_t i = 0; i < in_len; i += 4) {
        uint32_t n = (T[(unsigned char)in[i]] << 18) |
                     (T[(unsigned char)in[i+1]] << 12) |
                     (T[(unsigned char)in[i+2]] << 6) |
                     (T[(unsigned char)in[i+3]]);

        // we always write 3 bytes
        out[o++] = (n >> 16) & 0xFF;
        out[o++] = (n >> 8) & 0xFF;
        out[o++] = n & 0xFF;
    }

    // fix for padding '='
    if (in_len >= 2) {
        if (in[in_len - 1] == '=') out.pop_back();
        if (in[in_len - 2] == '=') out.pop_back();
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
