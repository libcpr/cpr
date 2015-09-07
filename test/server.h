#ifndef CPR_TEST_SERVER_H
#define CPR_TEST_SERVER_H

#include <string>

#include <gtest/gtest.h>

#include <cpr.h>

using namespace cpr;

class Server: public ::testing::Environment {
  public:
    virtual void SetUp();
    virtual void TearDown();

    Url GetBaseUrl();
};

#endif
