#ifndef CPR_SSLOPTIONS_H
#define CPR_SSLOPTIONS_H

namespace cpr {

/**
 * @brief SLL certificate verification option.
 *
 * Curl related options:
 * - https://curl.haxx.se/libcurl/c/CURLOPT_SSL_VERIFYPEER.html
 * - https://curl.haxx.se/libcurl/c/CURLOPT_SSL_VERIFYHOST.html
 */
class VerifySsl {
  public:
    VerifySsl() {}
    /**
     * @param [in] verifyPeer - verify the authenticity of the peer's certificate
     * @param [in] verifyHost - verify that the server certificate is for the server it is known as
     */
    VerifySsl(bool verifyPeer, bool verifyHost = true);

    bool isVerifyPeerOn() const;
    bool isVerifyHostOn() const;

    operator bool() const;

  private:
    bool verifyPeer_{true};
    bool verifyHost_{true};
};

} // namespace cpr

#endif
