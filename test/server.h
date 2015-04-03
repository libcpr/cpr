#ifndef CPR_TEST_SERVER_H
#define CPR_TEST_SERVER_H

#include <string>

#include <gtest/gtest.h>


class Server: public ::testing::Environment {
  public:
    virtual void SetUp();
    virtual void TearDown();
};

#endif
