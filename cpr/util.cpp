#include "util.h"

#include <sstream>
#include <vector>


Header cpr::util::parseHeader(const std::string& headers) {
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
            header.clear();
        }
        
        if (line.length() > 0) {
            auto found = line.find(":");
            if (found != std::string::npos) {
                auto value = line.substr(found + 2, line.length() - 1);
                if (value.back() == '\r') {
                    value = value.substr(0, value.length() - 1);
                }
                header[line.substr(0, found)] = value;
            }
        }
    }

    return header;
}

std::string cpr::util::parseResponse(const std::string& response) {
    if (!response.empty()) {
        if (response.back() == '\n') {
            return response.substr(0, response.length() - 1);
        }
    }

    return response;
}

size_t cpr::util::writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
    data->append((char*) ptr, size * nmemb);
    return size * nmemb;
}
