module;

#define CPR_AS_MODULE
#include "cpr/cpr.h"
#include "cpr/secure_string.h"

export module cpr;

export namespace cpr {
    using cpr::AcceptEncodingMethods;
    using cpr::AcceptEncoding;
    using cpr::AsyncResponse;
    using cpr::Get;
    using cpr::GetAsync;
    using cpr::GetCallback;
    using cpr::Post;
    using cpr::PostAsync;
    using cpr::PostCallback;
    using cpr::Put;
    using cpr::PutAsync;
    using cpr::PutCallback;
    using cpr::Head;
    using cpr::HeadAsync;
    using cpr::HeadCallback;
    using cpr::Delete;
    using cpr::DeleteAsync;
    using cpr::DeleteCallback;
    using cpr::Options;
    using cpr::OptionsAsync;
    using cpr::OptionsCallback;
    using cpr::Patch;
    using cpr::PatchAsync;
    using cpr::PatchCallback;
    using cpr::Download;
    using cpr::DownloadAsync;
    using cpr::MultiGet;
    using cpr::MultiDelete;
    using cpr::MultiPut;
    using cpr::MultiHead;
    using cpr::MultiOptions;
    using cpr::MultiPatch;
    using cpr::MultiPost;
    using cpr::MultiGetAsync;
    using cpr::MultiDeleteAsync;
    using cpr::MultiPutAsync;
    using cpr::MultiHeadAsync;
    using cpr::MultiOptionsAsync;
    using cpr::MultiPatchAsync;
    using cpr::MultiPostAsync;
    using cpr::MultiPutAsync;
    using cpr::CancellationResult;
    using cpr::AsyncWrapper;
    using cpr::GlobalThreadPool;
    using cpr::AuthMode;
    using cpr::Authentication;
    #if LIBCURL_VERSION_NUM >= 0x073D00
    using cpr::Bearer;
    #endif
    using cpr::BodyView;
    using cpr::Body;
    using cpr::Buffer;
    using cpr::ReadCallback;
    using cpr::HeaderCallback;
    using cpr::WriteCallback;
    using cpr::ProgressCallback;
    using cpr::DebugCallback;
    using cpr::CancellationCallback;
    using cpr::CertInfo;
    using cpr::ConnectTimeout;
    using cpr::ConnectionPool;
    using cpr::Cookie;
    using cpr::Cookies;
    using cpr::StringHolder;
    using cpr::Url;
    using cpr::Parameter;
    using cpr::Pair;
    using cpr::CurlContainer;
    using cpr::CurlHolder;
    using cpr::CurlMultiHolder;
    using cpr::ErrorCode;
    using cpr::Error;
    using cpr::File;
    using cpr::Files;
    using cpr::HttpVersionCode;
    using cpr::HttpVersion;
    using cpr::Interceptor;
    using cpr::InterceptorMulti;
    using cpr::Interface;
    using cpr::LimitRate;
    using cpr::LocalPortRange;
    using cpr::LocalPort;
    using cpr::LowSpeed;
    using cpr::Part;
    using cpr::Multipart;
    using cpr::InterceptorMulti;
    using cpr::MultiPerform;
    using cpr::Parameters;
    using cpr::Payload;
    using cpr::Proxies;
    using cpr::ProxyAuthentication;
    using cpr::EncodedAuthentication;
    using cpr::Range;
    using cpr::MultiRange;
    using cpr::PostRedirectFlags;
    using cpr::Redirect;
    using cpr::ReserveSize;
    using cpr::Resolve;
    using cpr::Response;
    using cpr::Content;
    using cpr::Session;
    using cpr::ServerSentEvent;
    using cpr::ServerSentEventParser;
    using cpr::ServerSentEventCallback;
    #if SUPPORT_CURLOPT_SSL_CTX_FUNCTION
    using cpr::sslctx_function_load_ca_cert_from_buffer;
    #endif
    using cpr::VerifySsl;

    namespace ssl {
        using cpr::ssl::CertFile;
        using cpr::ssl::PemCert;
        using cpr::ssl::DerCert;
        #if SUPPORT_CURLOPT_SSLCERT_BLOB
        using cpr::ssl::CertBlob;
        using cpr::ssl::PemBlob;
        using cpr::ssl::DerBlob;
        #endif
        using cpr::ssl::KeyFile;
        #if SUPPORT_CURLOPT_SSLKEY_BLOB
        using cpr::ssl::KeyBlob;
        #endif
        using cpr::ssl::PemKey;
        using cpr::ssl::DerKey;
        using cpr::ssl::PinnedPublicKey;
        #if SUPPORT_ALPN
        using cpr::ssl::ALPN;
        #endif
        #if SUPPORT_NPN
        using cpr::ssl::NPN;
        #endif
        using cpr::ssl::VerifyHost;
        using cpr::ssl::VerifyPeer;
        using cpr::ssl::VerifyStatus;
        using cpr::ssl::TLSv1;
        #if SUPPORT_SSLv2
        using cpr::ssl::SSLv2;
        #endif
        #if SUPPORT_SSLv3
        using cpr::ssl::SSLv3;
        #endif
        #if SUPPORT_TLSv1_0
        using cpr::ssl::TLSv1_0;
        #endif
        #if SUPPORT_TLSv1_1
        using cpr::ssl::TLSv1_1;
        #endif
        #if SUPPORT_TLSv1_2
        using cpr::ssl::TLSv1_2;
        #endif
        #if SUPPORT_TLSv1_3
        using cpr::ssl::TLSv1_3;
        #endif
        #if SUPPORT_MAX_TLS_VERSION
        using cpr::ssl::MaxTLSVersion;
        #endif
        #if SUPPORT_MAX_TLSv1_0
        using cpr::ssl::MaxTLSv1_0;
        #endif
        #if SUPPORT_MAX_TLSv1_1
        using cpr::ssl::MaxTLSv1_1;
        #endif
        #if SUPPORT_MAX_TLSv1_2
        using cpr::ssl::MaxTLSv1_2;
        #endif
        #if SUPPORT_MAX_TLSv1_3
        using cpr::ssl::MaxTLSv1_3;
        #endif
        using cpr::ssl::CaInfo;
        #if SUPPORT_CURLOPT_CAINFO_BLOB
        using cpr::ssl::CaInfoBlob;
        #endif
        using cpr::ssl::CaPath;
        #if SUPPORT_CURLOPT_SSL_CTX_FUNCTION
        using cpr::ssl::CaBuffer;
        #endif
        using cpr::ssl::Crl;
        using cpr::ssl::Ciphers;
        #if SUPPORT_TLSv13_CIPHERS
        using cpr::ssl::TLS13_Ciphers;
        #endif
        #if SUPPORT_SESSIONID_CACHE
        using cpr::ssl::SessionIdCache;
        #endif
        #if SUPPORT_SSL_FALSESTART
        using cpr::ssl::SslFastStart;
        #endif
        using cpr::ssl::NoRevoke;
    }

    using cpr::SslOptions;
    using cpr::ThreadPool;
    using cpr::Timeout;
    using cpr::UnixSocket;
    using cpr::UserAgent;
    using cpr::Verbose;

    using cpr::cpr_off_t;
    using cpr::cpr_pf_arg_t;

    using cpr::async;
    using cpr::get_error_code_to_string_mapping;
    using cpr::Ssl;

    namespace status {
        using cpr::status::HTTP_CONTINUE;
        using cpr::status::HTTP_SWITCHING_PROTOCOL;
        using cpr::status::HTTP_PROCESSING;
        using cpr::status::HTTP_EARLY_HINTS;
        using cpr::status::HTTP_OK;
        using cpr::status::HTTP_CREATED;
        using cpr::status::HTTP_ACCEPTED;
        using cpr::status::HTTP_NON_AUTHORITATIVE_INFORMATION;
        using cpr::status::HTTP_NO_CONTENT;
        using cpr::status::HTTP_RESET_CONTENT;
        using cpr::status::HTTP_PARTIAL_CONTENT;
        using cpr::status::HTTP_MULTI_STATUS;
        using cpr::status::HTTP_ALREADY_REPORTED;
        using cpr::status::HTTP_IM_USED;
        using cpr::status::HTTP_MULTIPLE_CHOICE;
        using cpr::status::HTTP_MOVED_PERMANENTLY;
        using cpr::status::HTTP_FOUND;
        using cpr::status::HTTP_SEE_OTHER;
        using cpr::status::HTTP_NOT_MODIFIED;
        using cpr::status::HTTP_USE_PROXY;
        using cpr::status::HTTP_UNUSED;
        using cpr::status::HTTP_TEMPORARY_REDIRECT;
        using cpr::status::HTTP_PERMANENT_REDIRECT;
        using cpr::status::HTTP_BAD_REQUEST;
        using cpr::status::HTTP_UNAUTHORIZED;
        using cpr::status::HTTP_PAYMENT_REQUIRED;
        using cpr::status::HTTP_FORBIDDEN;
        using cpr::status::HTTP_NOT_FOUND;
        using cpr::status::HTTP_METHOD_NOT_ALLOWED;
        using cpr::status::HTTP_NOT_ACCEPTABLE;
        using cpr::status::HTTP_PROXY_AUTHENTICATION_REQUIRED;
        using cpr::status::HTTP_REQUEST_TIMEOUT;
        using cpr::status::HTTP_CONFLICT;
        using cpr::status::HTTP_GONE;
        using cpr::status::HTTP_LENGTH_REQUIRED;
        using cpr::status::HTTP_PRECONDITION_FAILED;
        using cpr::status::HTTP_PAYLOAD_TOO_LARGE;
        using cpr::status::HTTP_URI_TOO_LONG;
        using cpr::status::HTTP_UNSUPPORTED_MEDIA_TYPE;
        using cpr::status::HTTP_REQUESTED_RANGE_NOT_SATISFIABLE;
        using cpr::status::HTTP_EXPECTATION_FAILED;
        using cpr::status::HTTP_IM_A_TEAPOT;
        using cpr::status::HTTP_MISDIRECTED_REQUEST;
        using cpr::status::HTTP_UNPROCESSABLE_ENTITY;
        using cpr::status::HTTP_LOCKED;
        using cpr::status::HTTP_FAILED_DEPENDENCY;
        using cpr::status::HTTP_TOO_EARLY;
        using cpr::status::HTTP_UPGRADE_REQUIRED;
        using cpr::status::HTTP_PRECONDITION_REQUIRED;
        using cpr::status::HTTP_TOO_MANY_REQUESTS;
        using cpr::status::HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE;
        using cpr::status::HTTP_UNAVAILABLE_FOR_LEGAL_REASONS;
        using cpr::status::HTTP_INTERNAL_SERVER_ERROR;
        using cpr::status::HTTP_NOT_IMPLEMENTED;
        using cpr::status::HTTP_BAD_GATEWAY;
        using cpr::status::HTTP_SERVICE_UNAVAILABLE;
        using cpr::status::HTTP_GATEWAY_TIMEOUT;
        using cpr::status::HTTP_HTTP_VERSION_NOT_SUPPORTED;
        using cpr::status::HTTP_VARIANT_ALSO_NEGOTIATES;
        using cpr::status::HTTP_INSUFFICIENT_STORAGE;
        using cpr::status::HTTP_LOOP_DETECTED;
        using cpr::status::HTTP_NOT_EXTENDED;
        using cpr::status::HTTP_NETWORK_AUTHENTICATION_REQUIRED;
        using cpr::status::INFO_CODE_OFFSET;
        using cpr::status::SUCCESS_CODE_OFFSET;
        using cpr::status::REDIRECT_CODE_OFFSET;
        using cpr::status::CLIENT_ERROR_CODE_OFFSET;
        using cpr::status::SERVER_ERROR_CODE_OFFSET;
        using cpr::status::MISC_CODE_OFFSET;

        using cpr::status::is_informational;
        using cpr::status::is_success;
        using cpr::status::is_redirect;
        using cpr::status::is_client_error;
        using cpr::status::is_server_error;
    }

    namespace util {
        using cpr::util::SecureAllocator;
        using cpr::util::SecureString;

        using cpr::util::parseHeader;
        using cpr::util::parseCookies;
        using cpr::util::readUserFunction;
        using cpr::util::headerUserFunction;
        using cpr::util::writeFunction;
        using cpr::util::writeFileFunction;
        using cpr::util::writeUserFunction;
        using cpr::util::writeSSEFunction;
        using cpr::util::progressUserFunction;
        using cpr::util::debugUserFunction;
        using cpr::util::split;
        using cpr::util::urlEncode;
        using cpr::util::urlDecode;
        using cpr::util::isTrue;
        using cpr::util::sTimestampToT;

        using cpr::util::operator==;
        using cpr::util::operator!=;
    }

    using cpr::operator<<;
    using cpr::operator|;
    using cpr::operator&;
    using cpr::operator^;
    using cpr::operator~;
    using cpr::operator|=;
    using cpr::operator&=;
    using cpr::operator^=;
    using cpr::any;

    using cpr::AcceptEncodingMethodsStringMap;
    using cpr::EXPIRES_STRING_SIZE;
    using ::CPR_DEFAULT_THREAD_POOL_MAX_IDLE_TIME;
    using ::CPR_DEFAULT_THREAD_POOL_MIN_THREAD_NUM;
}

export namespace std {
    using std::to_string;
}
