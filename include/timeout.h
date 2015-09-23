#ifndef TIMEOUT_H
#define TIMEOUT_H

namespace cpr {

    class Timeout {
      public:
        Timeout(const long& timeout) : ms(timeout) {}

        long ms;
    };

} // namespace cpr

#endif
