#ifndef CPR_TEST_SERVER_H
#define CPR_TEST_SERVER_H

#include <string>

#include <gtest/gtest.h>

#include <cpr/cpr.h>

using namespace cpr;

class Server : public ::testing::Environment {
  public:
    Server() {}
    Server(const std::string cert_file) : cert_file(cert_file) {}

    virtual void SetUp();
    virtual void TearDown();

    Url GetBaseUrl();
    Url GetBaseUrlSSL();

    std::string cert_file;
};

#endif
