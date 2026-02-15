#include <gtest/gtest.h>

#include <utility>

#include "cpr/curlholder.h"

// Check if there is a double free in curl holder after move.
// To reproduce this, run with address sanitizers enabled.
// https://github.com/libcpr/cpr/issues/1286
TEST(CurlholderTests, MoveOperator) {
    cpr::CurlHolder a;
    cpr::CurlHolder b;

    a = std::move(b);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
