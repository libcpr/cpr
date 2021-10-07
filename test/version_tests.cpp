#include <cctype>
#include <cpr/cpr.h>
#include <cstddef>
#include <gtest/gtest.h>
#include <string>


TEST(VersionTests, StringVersionExists) {
#ifndef CPR_VERSION
    EXPECT_TRUE(false);
#endif // CPR_VERSION
}

TEST(VersionTests, StringVersionValid) {
    EXPECT_TRUE(CPR_VERSION != nullptr);
    std::string version = CPR_VERSION;

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

TEST(VersionTests, VersionMajorExists) {
#ifndef CPR_VERSION_MAJOR
    EXPECT_TRUE(false);
#endif // CPR_VERSION_MAJOR
}

TEST(VersionTests, VersionMinorExists) {
#ifndef CPR_VERSION_MINOR
    EXPECT_TRUE(false);
#endif // CPR_VERSION_MINOR
}

TEST(VersionTests, VersionPatchExists) {
#ifndef CPR_VERSION_PATCH
    EXPECT_TRUE(false);
#endif // CPR_VERSION_PATCH
}

TEST(VersionTests, VersionNumExists) {
#ifndef CPR_VERSION_NUM
    EXPECT_TRUE(false);
#endif // CPR_VERSION_NUM
}


int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
