#include "cpr/ssl_options.h"

using namespace cpr;

VerifySsl::VerifySsl(bool verifyPeer, bool verifyHost)
        : verifyPeer_{verifyPeer}, verifyHost_{verifyHost} {}

bool VerifySsl::GetVerifyPeer() const {
    return verifyPeer_;
}

bool VerifySsl::GetVerifyHost() const {
    return verifyHost_;
}

VerifySsl::operator bool() const {
    return verifyPeer_ && verifyHost_;
}
