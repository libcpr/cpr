#include "cpr/ssl_options.h"

namespace cpr {

VerifySsl::VerifySsl(bool verifyPeer, bool verifyHost)
        : verifyPeer_{verifyPeer}, verifyHost_{verifyHost} {}

bool VerifySsl::isVerifyPeerOn() const {
    return verifyPeer_;
}

bool VerifySsl::isVerifyHostOn() const {
    return verifyHost_;
}

VerifySsl::operator bool() const {
    return verifyPeer_ && verifyHost_;
}

} // namespace cpr
