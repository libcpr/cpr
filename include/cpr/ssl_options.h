#ifndef CPR_SSLOPTIONS_H
#define CPR_SSLOPTIONS_H

namespace cpr {

/**
 * SLL certificate verification option
 * Curl related options:
 * - https://curl.haxx.se/libcurl/c/CURLOPT_SSL_VERIFYPEER.html
 * - https://curl.haxx.se/libcurl/c/CURLOPT_SSL_VERIFYHOST.html
 */
class VerifySsl {
  public:
    VerifySsl() = default;

    /**
     * C-tor with verification options
     * verifyHost is on by default, for security and backward compatibility
     * @param[in] verifyPeer - verify the authenticity of the peer's certificate
     * @param[in] verifyHost - verify that the server certificate is for the server it is known as
     */
    VerifySsl(bool verifyPeer, bool verifyHost = true);

    /**
     * Get peer verification option
     * @return true if peer verification on, false otherwise
     */
    bool GetVerifyPeer() const;

    /**
    * Get host verification option
    * @return true if host verification on, false otherwise
    */
    bool GetVerifyHost() const;

    operator bool() const;

  private:
    bool verifyPeer_{true};
    bool verifyHost_{true};
};

} // namespace cpr

#endif
