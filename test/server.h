#ifndef _CPR_TEST_SERVER_H_
#define _CPR_TEST_SERVER_H_

#include <string>

#include <gtest/gtest.h>


class Server: public ::testing::Environment {
  public:
    virtual void SetUp();
    virtual void TearDown();
};

#endif
