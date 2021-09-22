#ifndef CPR_CPR_EXCEPTIONS_H
#define CPR_CPR_EXCEPTIONS_H
#include <exception>
#include <string>

namespace cpr {
    namespace Exception {
        class BadResponse : public std::exception {
            public:
                int http_code;
                std::string message;
                BadResponse(const std::string& msg, int http_code) : http_code(http_code), message(msg) {}
                ~BadResponse() {}
        };
        
    } // namespace Exception
} // namespace cpr

#endif