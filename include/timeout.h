#ifndef CPR_TIMEOUT_H
#define CPR_TIMEOUT_H

namespace cpr {

class Timeout {
  public:
    Timeout(const long& timeout) : ms(timeout) {}

    long ms;
};

} // namespace cpr

#endif
