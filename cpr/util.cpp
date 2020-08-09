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

    for (auto& line : lines) {
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
            size_t found = line.find(":");
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

size_t writeFunction(void* ptr, size_t size, size_t nmemb, std::string* data) {
    data->append(static_cast<char*>(ptr), size * nmemb);
    return size * nmemb;
}

size_t downloadFunction(void* ptr, size_t size, size_t nmemb, std::ofstream* file) {
    file->write((char*) ptr, size * nmemb);
    return size * nmemb;
}

/**
 * Creates a temporary CurlHolder object and uses it to escape the given string.
 * If you plan to use this methode on a regular basis think about creating CurlHolder
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

} // namespace util
} // namespace cpr
