#include "util.h"

#include <sstream>
#include <vector>


Header cpr::util::parseHeader(std::string headers) {
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
                header[line.substr(0, found)] = line.substr(found + 2, line.length());
            }
        }
    }

    return header;
}

std::string cpr::util::parseResponse(std::string response) {
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

void cpr::util::freeHolder(CurlHolder* holder) {
    curl_easy_cleanup(holder->handle);
    curl_slist_free_all(holder->chunk);
}

CurlHolder* cpr::util::newHolder() {
    CurlHolder* holder = new CurlHolder();
    holder->handle = curl_easy_init();
    return holder;
}
