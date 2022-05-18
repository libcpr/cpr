#ifndef CPR_LOCAL_PORT_RANGE_H
#define CPR_LOCAL_PORT_RANGE_H

namespace cpr {

class LocalPortRange {
  public:
    // NOLINTNEXTLINE(google-explicit-constructor)
    LocalPortRange(const long p_localportrange) : localportrange_(p_localportrange) {}

    operator long() const {
        return localportrange_;
    }

  private:
    long localportrange_ = 0;
};

} // namespace cpr

#endif