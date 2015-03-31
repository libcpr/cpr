#ifndef _CPR_TYPES_H_
#define _CPR_TYPES_H_

#include <algorithm>
#include <map>
#include <string>

#include <curl/curl.h>


struct case_insensitive_compare
{
    case_insensitive_compare() {}

    bool operator()(const std::string& a, const std::string& b) const {
        return to_lower(a) < to_lower(b);
    }

    static void char_to_lower(char& c) {
        if (c >= 'A' && c <= 'Z')
        c += ('a' - 'A');
    }

    static std::string to_lower(const std::string& a) {
        std::string s(a);
        std::for_each(s.begin(), s.end(), char_to_lower);
        return s;
    }
};

typedef std::map<std::string, std::string> Parameters;
typedef std::map<std::string, std::string, case_insensitive_compare> Header;
typedef std::string Url;
typedef std::map<std::string, std::string> Payload;
typedef long Timeout;

typedef struct {
    CURL* handle;
    struct curl_slist* chunk;
} CurlHolder;

#endif
