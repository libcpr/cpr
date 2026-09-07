#include "cpr/cpr.h"
#include <cctype>
#include <cstddef>
#include <gtest/gtest.h>
#include <string>


TEST(VersionTests, StringVersionValid) {
    std::string version{CPR_VERSION};

    // Check if the version string is: '\d+\.\d+\.\d+'
    bool digit = true;
    size_t dotCount = 0;
    for (size_t i = 0; i < version.size(); i++) {
        if (i == 0) {
            EXPECT_TRUE(std::isdigit(version[i]));
        } else if (digit) {
            if (version[i] == '.') {
                digit = false;
                dotCount++;
                continue;
            }
        }
        EXPECT_TRUE(std::isdigit(version[i]));
        digit = true;
    }
    EXPECT_EQ(dotCount, 2);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
