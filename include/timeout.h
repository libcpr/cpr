#ifndef TIMEOUT_H
#define TIMEOUT_H


class Timeout {
  public:
    Timeout(const long& timeout) : ms(timeout) {}

    long ms;
};

#endif
