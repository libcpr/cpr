#include "cpr/ssl_options.h"
#include "cpr/util.h"

namespace cpr {
namespace ssl {
KeyFile::~KeyFile() {
    util::secureStringClear(password);
}

KeyBlob::~KeyBlob() {
    util::secureStringClear(password);
}
} // namespace ssl

SslOptions::~SslOptions() noexcept {
#if SUPPORT_CURLOPT_SSLKEY_BLOB
    util::secureStringClear(key_blob);
#endif
    util::secureStringClear(key_pass);
}
} // namespace cpr