#ifndef CPR_CURLMULTIHOLDER_H
#define CPR_CURLMULTIHOLDER_H

#include "cpr/export.h"

#include <curl/curl.h>

namespace cpr {

EXPORT_CPR class CurlMultiHolder {
  public:
    CurlMultiHolder();
    ~CurlMultiHolder();

    CURLM* handle{nullptr};
};

} // namespace cpr

#endif
