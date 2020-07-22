#ifndef CPR_SSLOPTIONS_H
#define CPR_SSLOPTIONS_H

#include <string>

#include <curl/curl.h>

#include <utility>

#define __LIBCURL_VERSION_GTE(major, minor) \
    ((LIBCURL_VERSION_MAJOR > (major)) ||   \
     ((LIBCURL_VERSION_MAJOR == (major)) && (LIBCURL_VERSION_MINOR >= (minor))))
#define __LIBCURL_VERSION_LT(major, minor) \
    ((LIBCURL_VERSION_MAJOR < (major)) ||  \
     ((LIBCURL_VERSION_MAJOR == (major)) && (LIBCURL_VERSION_MINOR < (minor))))

#ifndef SUPPORT_ALPN
#define SUPPORT_ALPN __LIBCURL_VERSION_GTE(7, 36)
#endif
#ifndef SUPPORT_NPN
#define SUPPORT_NPN __LIBCURL_VERSION_GTE(7, 36)
#endif

#ifndef SUPPORT_SSLv2
#define SUPPORT_SSLv2 __LIBCURL_VERSION_LT(7, 19)
#endif
#ifndef SUPPORT_SSLv3
#define SUPPORT_SSLv3 __LIBCURL_VERSION_LT(7, 39)
#endif
#ifndef SUPPORT_TLSv1_0
#define SUPPORT_TLSv1_0 __LIBCURL_VERSION_GTE(7, 34)
#endif
#ifndef SUPPORT_TLSv1_1
#define SUPPORT_TLSv1_1 __LIBCURL_VERSION_GTE(7, 34)
#endif
#ifndef SUPPORT_TLSv1_2
#define SUPPORT_TLSv1_2 __LIBCURL_VERSION_GTE(7, 34)
#endif
#ifndef SUPPORT_TLSv1_3
#define SUPPORT_TLSv1_3 __LIBCURL_VERSION_GTE(7, 52)
#endif
#ifndef SUPPORT_MAX_TLS_VERSION
#define SUPPORT_MAX_TLS_VERSION __LIBCURL_VERSION_GTE(7, 54)
#endif
#ifndef SUPPORT_MAX_TLSv1_1
#define SUPPORT_MAX_TLSv1_1 __LIBCURL_VERSION_GTE(7, 54)
#endif
#ifndef SUPPORT_MAX_TLSv1_2
#define SUPPORT_MAX_TLSv1_2 __LIBCURL_VERSION_GTE(7, 54)
#endif
#ifndef SUPPORT_MAX_TLSv1_3
#define SUPPORT_MAX_TLSv1_3 __LIBCURL_VERSION_GTE(7, 54)
#endif
#ifndef SUPPORT_TLSv13_CIPHERS
#define SUPPORT_TLSv13_CIPHERS __LIBCURL_VERSION_GTE(7, 61)
#endif
#ifndef SUPPORT_SESSIONID_CACHE
#define SUPPORT_SESSIONID_CACHE __LIBCURL_VERSION_GTE(7, 16)
#endif
#ifndef SUPPORT_SSL_FALSESTART
#define SUPPORT_SSL_FALSESTART __LIBCURL_VERSION_GTE(7, 42)
#endif

namespace cpr {

class VerifySsl {
  public:
    VerifySsl() {}
    VerifySsl(bool verify) : verify(verify) {}

    operator bool() const {
        return verify;
    }

    bool verify = true;
};

namespace ssl {

// set SSL client certificate
class CertFile {
  public:
    CertFile(std::string&& p_filename) : filename(std::move(p_filename)) {}

    const std::string filename;

    virtual const char* GetCertType(void) const {
        return "PEM";
    }
};

typedef CertFile PemCert;

class DerCert : public CertFile {
  public:
    DerCert(std::string&& p_filename) : CertFile(std::move(p_filename)) {}

    virtual const char* GetCertType(void) const {
        return "DER";
    }
};

// specify private keyfile for TLS and SSL client cert
class KeyFile {
  public:
    KeyFile(std::string&& p_filename) : filename(std::move(p_filename)) {}

    template <typename FileType, typename PassType>
    KeyFile(FileType&& p_filename, PassType p_password)
            : filename(std::move(p_filename)), password(std::move(p_password)) {}

    std::string filename;
    std::string password;

    virtual const char* GetKeyType(void) const {
        return "PEM";
    }
};

typedef KeyFile PemKey;

class DerKey : public KeyFile {
  public:
    DerKey(std::string&& p_filename) : KeyFile(std::move(p_filename)) {}

    template <typename FileType, typename PassType>
    DerKey(FileType&& p_filename, PassType p_password)
            : KeyFile(std::move(p_filename), std::move(p_password)) {}

    virtual const char* GetKeyType(void) const {
        return "DER";
    }
};

#if SUPPORT_ALPN
// This option enables/disables ALPN in the SSL handshake (if the SSL backend libcurl is built to
// use supports it), which can be used to negotiate http2.
class ALPN {
  public:
    ALPN() {}
    ALPN(bool enabled) : enabled(enabled) {}

    operator bool() const {
        return enabled;
    }

    bool enabled = true;
};
#endif // SUPPORT_ALPN

#if SUPPORT_NPN
//  This option enables/disables NPN in the SSL handshake (if the SSL backend libcurl is built to
//  use supports it), which can be used to negotiate http2.
class NPN {
  public:
    NPN() {}
    NPN(bool enabled) : enabled(enabled) {}

    operator bool() const {
        return enabled;
    }

    bool enabled = true;
};
#endif // SUPPORT_NPN

// This option determines whether libcurl verifies that the server cert is for the server it is
// known as.
class VerifyHost {
  public:
    VerifyHost() {}
    VerifyHost(bool enabled) : enabled(enabled) {}

    operator bool() const {
        return enabled;
    }

    bool enabled = true;
};

// This option determines whether libcurl verifies the authenticity of the peer's certificate.
class VerifyPeer {
  public:
    VerifyPeer() {}
    VerifyPeer(bool enabled) : enabled(enabled) {}

    operator bool() const {
        return enabled;
    }

    bool enabled = true;
};

// This option determines whether libcurl verifies the status of the server cert using the
// "Certificate Status Request" TLS extension (aka. OCSP stapling).
class VerifyStatus {
  public:
    VerifyStatus(bool enabled) : enabled(enabled) {}

    operator bool() const {
        return enabled;
    }

    bool enabled = false;
};

// TLS v1.0 or later
struct TLSv1 {};
#if SUPPORT_SSLv2
// SSL v2 (but not SSLv3)
struct SSLv2 {};
#endif
#if SUPPORT_SSLv3
// SSL v3 (but not SSLv2)
struct SSLv3 {};
#endif
#if SUPPORT_TLSv1_0
// TLS v1.0 or later (Added in 7.34.0)
struct TLSv1_0 {};
#endif
#if SUPPORT_TLSv1_1
// TLS v1.1 or later (Added in 7.34.0)
struct TLSv1_1 {};
#endif
#if SUPPORT_TLSv1_2
// TLS v1.2 or later (Added in 7.34.0)
struct TLSv1_2 {};
#endif
#if SUPPORT_TLSv1_3
// TLS v1.3 or later (Added in 7.52.0)
struct TLSv1_3 {};
#endif
#if SUPPORT_MAX_TLS_VERSION
// The flag defines the maximum supported TLS version by libcurl, or the default value from the SSL
// library is used.
struct MaxTLSVersion {};
#endif
#if SUPPORT_MAX_TLSv1_0
// The flag defines maximum supported TLS version as TLSv1.0. (Added in 7.54.0)
struct MaxTLSv1_0 {};
#endif
#if SUPPORT_MAX_TLSv1_1
// The flag defines maximum supported TLS version as TLSv1.1. (Added in 7.54.0)
struct MaxTLSv1_1 {};
#endif
#if SUPPORT_MAX_TLSv1_2
// The flag defines maximum supported TLS version as TLSv1.2. (Added in 7.54.0)
struct MaxTLSv1_2 {};
#endif
#if SUPPORT_MAX_TLSv1_3
// The flag defines maximum supported TLS version as TLSv1.3. (Added in 7.54.0)
struct MaxTLSv1_3 {};
#endif

// path to Certificate Authority (CA) bundle
class CaInfo {
  public:
    CaInfo(std::string&& p_filename) : filename(std::move(p_filename)) {}

    std::string filename;
};

// specify directory holding CA certificates
class CaPath {
  public:
    CaPath(std::string&& p_filename) : filename(std::move(p_filename)) {}

    std::string filename;
};

// specify a Certificate Revocation List file
class Crl {
  public:
    Crl(std::string&& p_filename) : filename(std::move(p_filename)) {}

    std::string filename;
};

// specify ciphers to use for TLS
class Ciphers {
  public:
    template <typename T>
    Ciphers(T&& p_ciphers) : ciphers(std::move(p_ciphers)) {}

    std::string ciphers;
};

#if SUPPORT_TLSv13_CIPHERS
// specify ciphers suites to use for TLS 1.3
class TLS13_Ciphers {
  public:
    template <typename T>
    TLS13_Ciphers(T&& p_ciphers) : ciphers(std::move(p_ciphers)) {}

    std::string ciphers;
};
#endif

#if SUPPORT_SESSIONID_CACHE
// enable/disable use of the SSL session-ID cache
class SessionIdCache {
  public:
    SessionIdCache() {}
    SessionIdCache(bool enabled) : enabled(enabled) {}

    operator bool() const {
        return enabled;
    }

    bool enabled = true;
};
#endif

#if SUPPORT_SSL_FALSESTART
class SslFastStart {
  public:
    SslFastStart() {}
    SslFastStart(bool enabled) : enabled(enabled) {}

    operator bool() const {
        return enabled;
    }

    bool enabled = false;
};
#endif

} // namespace ssl

struct SslOptions {
    std::string cert_file;
    std::string cert_type;
    std::string key_file;
    std::string key_type;
    std::string key_pass;
#if SUPPORT_ALPN
    bool enable_alpn = true;
#endif // SUPPORT_ALPN
#if SUPPORT_NPN
    bool enable_npn = true;
#endif // SUPPORT_ALPN
    bool verify_host = true;
    bool verify_peer = true;
    bool verify_status = false;
    int ssl_version = CURL_SSLVERSION_DEFAULT;
#if SUPPORT_MAX_TLS_VERSION
    int max_version = CURL_SSLVERSION_MAX_DEFAULT;
#endif
    std::string ca_info;
    std::string ca_path;
    std::string crl_file;
    std::string ciphers;
#if SUPPORT_TLSv13_CIPHERS
    std::string tls13_ciphers;
#endif
#if SUPPORT_SESSIONID_CACHE
    bool session_id_cache = true;
#endif

    void SetOption(const ssl::CertFile& opt) {
        cert_file = opt.filename;
        cert_type = opt.GetCertType();
    }
    void SetOption(const ssl::KeyFile& opt) {
        key_file = opt.filename;
        key_type = opt.GetKeyType();
        key_pass = opt.password;
    }
#if SUPPORT_ALPN
    void SetOption(const ssl::ALPN& opt) {
        enable_alpn = opt.enabled;
    }
#endif // SUPPORT_ALPN
#if SUPPORT_NPN
    void SetOption(const ssl::NPN& opt) {
        enable_npn = opt.enabled;
    }
#endif // SUPPORT_NPN
    void SetOption(const ssl::VerifyHost& opt) {
        verify_host = opt.enabled;
    }
    void SetOption(const ssl::VerifyPeer& opt) {
        verify_peer = opt.enabled;
    }
    void SetOption(const ssl::VerifyStatus& opt) {
        verify_status = opt.enabled;
    }
    void SetOption(const ssl::TLSv1& /*opt*/) {
        ssl_version = CURL_SSLVERSION_TLSv1;
    }
#if SUPPORT_SSLv2
    void SetOption(const ssl::SSLv2& /*opt*/) {
        ssl_version = CURL_SSLVERSION_SSLv2;
    }
#endif
#if SUPPORT_SSLv3
    void SetOption(const ssl::SSLv3& /*opt*/) {
        ssl_version = CURL_SSLVERSION_SSLv3;
    }
#endif
#if SUPPORT_TLSv1_0
    void SetOption(const ssl::TLSv1_0& /*opt*/) {
        ssl_version = CURL_SSLVERSION_TLSv1_0;
    }
#endif
#if SUPPORT_TLSv1_1
    void SetOption(const ssl::TLSv1_1& /*opt*/) {
        ssl_version = CURL_SSLVERSION_TLSv1_1;
    }
#endif
#if SUPPORT_TLSv1_2
    void SetOption(const ssl::TLSv1_2& /*opt*/) {
        ssl_version = CURL_SSLVERSION_TLSv1_2;
    }
#endif
#if SUPPORT_TLSv1_3
    void SetOption(const ssl::TLSv1_3& /*opt*/) {
        ssl_version = CURL_SSLVERSION_TLSv1_3;
    }
#endif
#if SUPPORT_MAX_TLS_VERSION
    void SetOption(const ssl::MaxTLSVersion& /*opt*/) {
        max_version = CURL_SSLVERSION_DEFAULT;
    }
#endif
#if SUPPORT_MAX_TLSv1_0
    void SetOption(const ssl::MaxTLSv1_0& opt) {
        max_version = CURL_SSLVERSION_MAX_TLSv1_0;
    }
#endif
#if SUPPORT_MAX_TLSv1_1
    void SetOption(const ssl::MaxTLSv1_1& /*opt*/) {
        max_version = CURL_SSLVERSION_MAX_TLSv1_1;
    }
#endif
#if SUPPORT_MAX_TLSv1_2
    void SetOption(const ssl::MaxTLSv1_2& /*opt*/) {
        max_version = CURL_SSLVERSION_MAX_TLSv1_2;
    }
#endif
#if SUPPORT_MAX_TLSv1_3
    void SetOption(const ssl::MaxTLSv1_3& /*opt*/) {
        max_version = CURL_SSLVERSION_MAX_TLSv1_3;
    }
#endif
    void SetOption(const ssl::CaInfo& opt) {
        ca_info = opt.filename;
    }
    void SetOption(const ssl::CaPath& opt) {
        ca_path = opt.filename;
    }
    void SetOption(const ssl::Crl& opt) {
        crl_file = opt.filename;
    }
    void SetOption(const ssl::Ciphers& opt) {
        ciphers = opt.ciphers;
    }
#if SUPPORT_TLSv13_CIPHERS
    void SetOption(const ssl::TLS13_Ciphers& opt) {
        tls13_ciphers = opt.ciphers;
    }
#endif
#if SUPPORT_SESSIONID_CACHE
    void SetOption(const ssl::SessionIdCache& opt) {
        session_id_cache = opt.enabled;
    }
#endif
};

namespace priv {

template <typename T>
void set_ssl_option(SslOptions& opts, T&& t) {
    opts.SetOption(std::move(t));
}

template <typename T, typename... Ts>
void set_ssl_option(SslOptions& opts, T&& t, Ts&&... ts) {
    set_ssl_option(opts, std::move(t));
    set_ssl_option(opts, std::move(ts)...);
}

} // namespace priv

template <typename... Ts>
SslOptions Ssl(Ts&&... ts) {
    SslOptions opts;
    priv::set_ssl_option(opts, std::move(ts)...);
    return opts;
}

} // namespace cpr

#endif
