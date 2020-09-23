#ifndef CPR_CALLBACK_H
#define CPR_CALLBACK_H

#include "cprtypes.h"

#include <functional>

namespace cpr {

class ReadCallback {
  public:
    ReadCallback() {}
    ReadCallback(std::function<bool(char* buffer, size_t& size)> callback)
            : size{-1}, callback{callback} {}
    ReadCallback(cpr_off_t size, std::function<bool(char* buffer, size_t& size)> callback)
            : size{size}, callback{callback} {}

    cpr_off_t size;
    std::function<bool(char* buffer, size_t& size)> callback;
};

class HeaderCallback {
  public:
    HeaderCallback() {}
    HeaderCallback(std::function<bool(std::string header)> callback) : callback(callback) {}

    std::function<bool(std::string header)> callback;
};

class WriteCallback {
  public:
    WriteCallback() {}
    WriteCallback(std::function<bool(std::string data)> callback) : callback(callback) {}

    std::function<bool(std::string data)> callback;
};

class ProgressCallback {
  public:
    ProgressCallback() {}
    ProgressCallback(std::function<bool(size_t downloadTotal, size_t downloadNow,
                                        size_t uploadTotal, size_t uploadNow)>
                             callback)
            : callback(callback) {}

    std::function<bool(size_t downloadTotal, size_t downloadNow, size_t uploadTotal,
                       size_t uploadNow)>
            callback;
};

class DebugCallback {
  public:
    enum class InfoType {
        TEXT = 0,
        HEADER_IN = 1,
        HEADER_OUT = 2,
        DATA_IN = 3,
        DATA_OUT = 4,
        SSL_DATA_IN = 5,
        SSL_DATA_OUT = 6,
    };
    DebugCallback() {}
    DebugCallback(std::function<void(InfoType type, std::string data)> callback)
            : callback(callback) {}

    std::function<void(InfoType type, std::string data)> callback;
};

} // namespace cpr

#endif
