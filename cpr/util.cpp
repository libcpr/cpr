#include "cpr/util.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace cpr {
namespace util {

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
                line.resize(std::min(line.size(), line.find_last_not_of("\t\n\r ") + 1));
                if (status_line != nullptr) {
                    *status_line = line;
                }

                // set the reason if it was given
                if (reason != nullptr) {
                    auto pos1 = line.find_first_of("\t ");
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
            auto found = line.find(":");
            if (found != std::string::npos) {
                auto value = line.substr(found + 1);
                value.erase(0, value.find_first_not_of("\t "));
                value.resize(std::min(value.size(), value.find_last_not_of("\t\n\r ") + 1));
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

std::string urlEncode(const std::string& value) {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (auto i = value.cbegin(), n = value.cend(); i != n; ++i) {
        std::string::value_type c = (*i);
        // Keep alphanumeric and other accepted characters intact
        if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }
        // Any other characters are percent-encoded
        escaped << '%' << std::setw(2) << std::int32_t(static_cast<unsigned char>(c));
    }

    return escaped.str();
}

} // namespace util
} // namespace cpr
