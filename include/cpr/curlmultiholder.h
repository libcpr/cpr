#ifndef CPR_CURLMULTIHOLDER_H
#define CPR_CURLMULTIHOLDER_H

#include <curl/curl.h>

namespace cpr {

class CurlMultiHolder {
  public:
    CurlMultiHolder();
    ~CurlMultiHolder();

    CURLM* handle{nullptr};
};

} // namespace cpr

#endif
