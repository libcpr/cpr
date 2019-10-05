#ifndef CPR_TEST_SERVER_H
#define CPR_TEST_SERVER_H

#include <string>

#include <gtest/gtest.h>

#include <cpr/cpr.h>

using namespace cpr;

class Server: public ::testing::Environment {
  public:
    virtual void SetUp();
    virtual void TearDown();

    Url GetBaseUrl();
    Url GetBaseUrlSSL();
    size_t GetConnectionCount();
    void ResetConnectionCount();
};

#endif
