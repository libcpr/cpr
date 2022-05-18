#ifndef CPR_LOCAL_PORT_H
#define CPR_LOCAL_PORT_H

namespace cpr {

class LocalPort {
  public:
    // NOLINTNEXTLINE(google-explicit-constructor)
    LocalPort(const long p_localport) : localport_(p_localport) {}

    operator long() const {
        return localport_;
    }

  private:
    long localport_ = 0;
};

} // namespace cpr

#endif