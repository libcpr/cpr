#ifndef CPR_SSLOPTIONS_H
#define CPR_SSLOPTIONS_H

#include <string>

#include <curl/curl.h>

#include "defines.h"

namespace cpr {

class VerifySsl {
  public:
    VerifySsl() {}
    VerifySsl(bool verify);

    operator bool() const;

  private:
    bool verify_ = true;
};

namespace ssl {

// set SSL client certificate
class CertFile {
  public:
    template <typename FileType>
    CertFile(FileType&& p_filename) : filename(CPR_FWD(p_filename)) {}

    std::string filename;

    virtual const char* GetCertType(void) const {
        return "PEM";
    }
};

typedef CertFile PemCert;

class DerCert : public CertFile {
  public:
    template <typename FileType>
    DerCert(FileType&& p_filename) : CertFile(CPR_FWD(p_filename)) {}

    virtual const char* GetCertType(void) const {
        return "DER";
    }
};

// specify private keyfile for TLS and SSL client cert
class KeyFile {
  public:
    template <typename FileType>
    KeyFile(FileType&& p_filename) : filename(CPR_FWD(p_filename)) {}

    template <typename FileType, typename PassType>
    KeyFile(FileType&& p_filename, PassType p_password)
            : filename(CPR_FWD(p_filename)), password(CPR_FWD(p_password)) {}

    std::string filename;
    std::string password;

    virtual const char* GetKeyType(void) const {
        return "PEM";
    }
};

typedef KeyFile PemKey;

class DerKey : public KeyFile {
  public:
    template <typename FileType>
    DerKey(FileType&& p_filename) : KeyFile(CPR_FWD(p_filename)) {}

    template <typename FileType, typename PassType>
    DerKey(FileType&& p_filename, PassType p_password)
            : KeyFile(CPR_FWD(p_filename), CPR_FWD(p_password)) {}

    virtual const char* GetKeyType(void) const {
        return "DER";
    }
};

// This option enables/disables ALPN in the SSL handshake (if the SSL backend libcurl is built to
// use supports it), which can be used to negotiate http2.
class Alpn {
  public:
    Alpn(bool p_enabled = true) : enabled(p_enabled) {}

    bool enabled;
};

//  This option enables/disables NPN in the SSL handshake (if the SSL backend libcurl is built to
//  use supports it), which can be used to negotiate http2.
class Npn {
  public:
    Npn(bool p_enabled = true) : enabled(p_enabled) {}

    bool enabled;
};

// This option determines whether libcurl verifies that the server cert is for the server it is
// known as.
class VerifyHost {
  public:
    VerifyHost(bool p_enabled = true) : enabled(p_enabled) {}

    bool enabled;
};

// This option determines whether libcurl verifies the authenticity of the peer's certificate.
class VerifyPeer {
  public:
    VerifyPeer(bool p_enabled = true) : enabled(p_enabled) {}

    bool enabled;
};

// This option determines whether libcurl verifies the status of the server cert using the
// "Certificate Status Request" TLS extension (aka. OCSP stapling).
class VerifyStatus {
  public:
    VerifyStatus(bool p_enabled = true) : enabled(p_enabled) {}

    bool enabled;
};

struct TLSv1 {};
#if LIBCURL_VERSION_MAJOR >= 7
#if LIBCURL_VERSION_MINOR < 18
struct SSLv2 {};
#endif
#if LIBCURL_VERSION_MINOR < 39
struct SSLv3 {};
#endif
#if LIBCURL_VERSION_MINOR >= 34
struct TLSv1_0 {};
struct TLSv1_1 {};
struct TLSv1_2 {};
#endif // LIBCURL_VERSION_MINOR >= 34
#if LIBCURL_VERSION_MINOR >= 52
struct TLSv1_3 {};
#endif
#if LIBCURL_VERSION_MINOR >= 54
// The flag defines maximum supported TLS version as TLSv1.0. (Added in 7.54.0)
struct MaxTLSv1_0 {};
// The flag defines maximum supported TLS version as TLSv1.1. (Added in 7.54.0)
struct MaxTLSv1_1 {};
// The flag defines maximum supported TLS version as TLSv1.2. (Added in 7.54.0)
struct MaxTLSv1_2 {};
// The flag defines maximum supported TLS version as TLSv1.3. (Added in 7.54.0)
struct MaxTLSv1_3 {};
#endif
#endif // LIBCURL_VERSION_MAJOR >= 7

// path to Certificate Authority (CA) bundle
class CaInfo {
  public:
    template <typename FileType>
    CaInfo(FileType&& p_filename) : filename(CPR_FWD(p_filename)) {}

    std::string filename;
};

// specify directory holding CA certificates
class CaPath {
  public:
    template <typename FileType>
    CaPath(FileType&& p_filename) : filename(CPR_FWD(p_filename)) {}

    std::string filename;
};

// specify a Certificate Revocation List file
class Crl {
  public:
    template <typename FileType>
    Crl(FileType&& p_filename) : filename(CPR_FWD(p_filename)) {}

    std::string filename;
};

// specify ciphers to use for TLS
class Ciphers {
  public:
    template <typename T>
    Ciphers(T&& p_ciphers) : ciphers(CPR_FWD(p_ciphers)) {}

    std::string ciphers;
};

// enable/disable use of the SSL session-ID cache
class SessionIdCache {
  public:
    SessionIdCache(bool p_enabled = true) : enabled(p_enabled) {}

    bool enabled;
};

} // namespace ssl

struct SslOptions {
    std::string cert_file;
    std::string cert_type;
    std::string key_file;
    std::string key_type;
    std::string key_pass;
    bool enable_alpn;
    bool enable_npn;
    bool verify_host;
    bool verify_peer;
    bool verify_status;
    int ssl_version;
    int max_version;
    std::string ca_info;
    std::string ca_path;
    std::string crl_file;
    std::string ciphers;
    bool session_id_cache;

    SslOptions()
            : ssl_version(CURL_SSLVERSION_DEFAULT)
#if LIBCURL_VERSION_MAJOR >= 7
#if LIBCURL_VERSION_MINOR >= 54
              ,
              max_version(CURL_SSLVERSION_MAX_DEFAULT)
#endif
#endif
              ,
              session_id_cache(true) {
    }

    void SetOption(const ssl::CertFile& opt) {
        cert_file = opt.filename;
        cert_type = opt.GetCertType();
    }
    void SetOption(const ssl::KeyFile& opt) {
        key_file = opt.filename;
        key_type = opt.GetKeyType();
        key_pass = opt.password;
    }
    void SetOption(const ssl::Alpn& opt) {
        enable_alpn = opt.enabled;
    }
    void SetOption(const ssl::Npn& opt) {
        enable_npn = opt.enabled;
    }
    void SetOption(const ssl::VerifyHost& opt) {
        verify_host = opt.enabled;
    }
    void SetOption(const ssl::VerifyPeer& opt) {
        verify_peer = opt.enabled;
    }
    void SetOption(const ssl::VerifyStatus& opt) {
        verify_status = opt.enabled;
    }
    void SetOption(const ssl::TLSv1& opt) {
        ssl_version = CURL_SSLVERSION_TLSv1;
    }
#if LIBCURL_VERSION_MAJOR >= 7
#if LIBCURL_VERSION_MINOR < 18
    void SetOption(const ssl::SSLv2& opt) {
        ssl_version = CURL_SSLVERSION_SSLv2;
    }
#endif
#if LIBCURL_VERSION_MINOR < 39
    void SetOption(const ssl::SSLv3& opt) {
        ssl_version = CURL_SSLVERSION_SSLv3;
    }
#endif
#if LIBCURL_VERSION_MINOR >= 34
    void SetOption(const ssl::TLSv1_0& opt) {
        ssl_version = CURL_SSLVERSION_TLSv1_0;
    }
    void SetOption(const ssl::TLSv1_1& opt) {
        ssl_version = CURL_SSLVERSION_TLSv1_1;
    }
    void SetOption(const ssl::TLSv1_2& opt) {
        ssl_version = CURL_SSLVERSION_TLSv1_2;
    }
#endif // LIBCURL_VERSION_MINOR >= 34
#if LIBCURL_VERSION_MINOR >= 52
    void SetOption(const ssl::TLSv1_3& opt) {
        ssl_version = CURL_SSLVERSION_TLSv1_3;
    }
#endif
#if LIBCURL_VERSION_MINOR >= 54
    void SetOption(const ssl::MaxTLSv1_0& opt) {
        max_version = CURL_SSLVERSION_MAX_TLSv1_0;
    }
    void SetOption(const ssl::MaxTLSv1_1& opt) {
        max_version = CURL_SSLVERSION_MAX_TLSv1_1;
    }
    void SetOption(const ssl::MaxTLSv1_2& opt) {
        max_version = CURL_SSLVERSION_MAX_TLSv1_2;
    }
    void SetOption(const ssl::MaxTLSv1_3& opt) {
        max_version = CURL_SSLVERSION_MAX_TLSv1_3;
    }
#endif
#endif // LIBCURL_VERSION_MAJOR >= 7
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
    void SetOption(const ssl::SessionIdCache& opt) {
        session_id_cache = opt.enabled;
    }
};

namespace priv {

template <typename T>
void set_ssl_option(SslOptions& opts, T&& t) {
    opts.SetOption(CPR_FWD(t));
}

template <typename T, typename... Ts>
void set_ssl_option(SslOptions& opts, T&& t, Ts&&... ts) {
    set_ssl_option(opts, CPR_FWD(t));
    set_ssl_option(opts, CPR_FWD(ts)...);
}

} // namespace priv

template <typename... Ts>
SslOptions Ssl(Ts&&... ts) {
    SslOptions opts;
    priv::set_ssl_option(opts, CPR_FWD(ts)...);
    return opts;
}

} // namespace cpr

#endif
