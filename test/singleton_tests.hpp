#pragma once

#include "cpr/cpr.h"

class TestSingleton {
    CPR_SINGLETON_DECL(TestSingleton)
  private:
    TestSingleton() = default;
};
