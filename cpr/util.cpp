#include "cpr/util.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace cpr {
namespace util {

Cookies parseCookies(curl_slist* raw_cookies) {
    Cookies cookies;
    for (curl_slist* nc = raw_cookies; nc; nc = nc->next) {
        std::vector<std::string> tokens = cpr::util::split(nc->data, '\t');
        std::string value = tokens.back();
        tokens.pop_back();
        cookies[tokens.back()] = value;
    }
    return cookies;
}

Header parseHeader(const std::string& headers, std::string* status_line, std::string* reason) {
    Header header;
    std::vector<std::string> lines;
    std::istringstream stream(headers);
    {
        std::string line;
        while (std::getline(stream, line, '\n')) {
            lines.push_back(line);
        }
    }

    for (std::string& line : lines) {
        // NOLINTNEXTLINE (cppcoreguidelines-avoid-magic-numbers)
        if (line.substr(0, 5) == "HTTP/") {
            // set the status_line if it was given
            if ((status_line != nullptr) || (reason != nullptr)) {
                line.resize(std::min<size_t>(line.size(), line.find_last_not_of("\t\n\r ") + 1));
                if (status_line != nullptr) {
                    *status_line = line;
                }

                // set the reason if it was given
                if (reason != nullptr) {
                    size_t pos1 = line.find_first_of("\t ");
                    size_t pos2 = std::string::npos;
                    if (pos1 != std::string::npos) {
                        pos2 = line.find_first_of("\t ", pos1 + 1);
                    }
                    if (pos2 != std::string::npos) {
                        line.erase(0, pos2 + 1);
                        *reason = line;
                    }
                }
            }
            header.clear();
        }

        if (line.length() > 0) {
            size_t found = line.find(':');
            if (found != std::string::npos) {
                std::string value = line.substr(found + 1);
                value.erase(0, value.find_first_not_of("\t "));
                value.resize(std::min<size_t>(value.size(), value.find_last_not_of("\t\n\r ") + 1));
                header[line.substr(0, found)] = value;
            }
        }
    }

    return header;
}

std::vector<std::string> split(const std::string& to_split, char delimiter) {
    std::vector<std::string> tokens;

    std::stringstream stream(to_split);
    std::string item;
    while (std::getline(stream, item, delimiter)) {
        tokens.push_back(item);
    }

    return tokens;
}

size_t readUserFunction(char* ptr, size_t size, size_t nitems, const ReadCallback* read) {
    size *= nitems;
    return read->callback(ptr, size) ? size : CURL_READFUNC_ABORT;
}

size_t headerUserFunction(char* ptr, size_t size, size_t nmemb, const HeaderCallback* header) {
    size *= nmemb;
    return header->callback({ptr, size}) ? size : 0;
}

size_t writeFunction(char* ptr, size_t size, size_t nmemb, std::string* data) {
    size *= nmemb;
    data->append(ptr, size);
    return size;
}

size_t writeFileFunction(char* ptr, size_t size, size_t nmemb, std::ofstream* file) {
    size *= nmemb;
    file->write(ptr, size);
    return size;
}

size_t writeUserFunction(char* ptr, size_t size, size_t nmemb, const WriteCallback* write) {
    size *= nmemb;
    return write->callback({ptr, size}) ? size : 0;
}

#if LIBCURL_VERSION_NUM < 0x072000
int progressUserFunction(const ProgressCallback* progress, double dltotal, double dlnow,
                         double ultotal, double ulnow) {
#else
int progressUserFunction(const ProgressCallback* progress, curl_off_t dltotal, curl_off_t dlnow,
                         curl_off_t ultotal, curl_off_t ulnow) {
#endif
    return progress->callback(dltotal, dlnow, ultotal, ulnow) ? 0 : 1;
}

int debugUserFunction(CURL* /*handle*/, curl_infotype type, char* data, size_t size,
                      const DebugCallback* debug) {
    debug->callback(DebugCallback::InfoType(type), std::string(data, size));
    return 0;
}

/**
 * Creates a temporary CurlHolder object and uses it to escape the given string.
 * If you plan to use this methode on a regular basis think about creating a CurlHolder
 * object and calling urlEncode(std::string) on it.
 *
 * Example:
 * CurlHolder holder;
 * std::string input = "Hello World!";
 * std::string result = holder.urlEncode(input);
 **/
std::string urlEncode(const std::string& s) {
    CurlHolder holder; // Create a temporary new holder for URL encoding
    return holder.urlEncode(s);
}

/**
 * Creates a temporary CurlHolder object and uses it to unescape the given string.
 * If you plan to use this methode on a regular basis think about creating a CurlHolder
 * object and calling urlDecode(std::string) on it.
 *
 * Example:
 * CurlHolder holder;
 * std::string input = "Hello%20World%21";
 * std::string result = holder.urlDecode(input);
 **/
std::string urlDecode(const std::string& s) {
    CurlHolder holder; // Create a temporary new holder for URL decoding
    return holder.urlDecode(s);
}

} // namespace util
} // namespace cpr
