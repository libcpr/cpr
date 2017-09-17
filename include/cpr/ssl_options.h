#ifndef CPR_SSLOPTIONS_H
#define CPR_SSLOPTIONS_H

#include "cprtypes.h"

namespace cpr {

class SslCert : public StringHolder<SslCert> {
  public:
    using StringHolder<SslCert>::StringHolder;
};

class SslKey : public StringHolder<SslKey> {
  public:
    using StringHolder<SslKey>::StringHolder;
};

class SslKeyPass : public StringHolder<SslKeyPass> {
  public:
    using StringHolder<SslKeyPass>::StringHolder;
};

class VerifySsl {
  public:
    VerifySsl() {}
    VerifySsl(bool verify);

    operator bool() const;

  private:
    bool verify_ = true;
};

} // namespace cpr

#endif
