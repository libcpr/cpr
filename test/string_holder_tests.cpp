#include <gtest/gtest.h>

#include <string>
#include <vector>

#include <cpr/cpr.h>

using namespace cpr;

class Foo : public cpr::StringHolder<Foo> {
    using cpr::StringHolder<Foo>::StringHolder;
};

TEST(StringHolderTests, ConstructionTest) {
    std::string foo = "/foo";
    auto foo1 = Foo{foo};
    auto foo2 = Foo{"/foo"};
    auto foo3 = Foo{foo.c_str()};
    auto foo4 = Foo{foo3};

    EXPECT_EQ(foo1, foo2);
    EXPECT_EQ(foo2, foo3);
    EXPECT_EQ(foo3, foo4);
}

TEST(StringHolderTests, StringAPITest) {
    std::string str = "/foo";
    auto foo = Foo{str};

    EXPECT_EQ(foo.str(), str);
    EXPECT_EQ(*foo.c_str(), *str.c_str());
    EXPECT_EQ(*foo.data(), *str.data());
}

TEST(StringHolderTests, ConcatenationTest) {
    auto base = Foo{"/base"};
    std::string foo = "/foo";
    const char *bar = "/bar";
    auto baz = Foo{"/baz"};

    auto concat1 = base + foo;
    auto concat2 = base + bar;
    auto concat3 = base + baz;

    EXPECT_EQ(concat1.str(), base.str() + foo);
    EXPECT_EQ(concat2.str(), base.str() + bar);
    EXPECT_EQ(concat3.str(), base.str() + baz.str());
}

TEST(StringHolderTests, AppendTest) {
    auto base = Foo{"/base"};
    std::string foo = "/foo";
    const char *bar = "/bar";
    auto baz = Foo{"/baz"};

    auto append1 = base;
    append1 += foo;

    auto append2 = base;
    append2 += bar;

    auto append3 = base;
    append3 += baz;

    EXPECT_EQ(append1, base.str() + foo);
    EXPECT_EQ(append2, base.str() + bar);
    EXPECT_EQ(append3, base.str() + baz.str());
}

TEST(StringHolderTests, EqualityTest) {
    auto control = Foo{"/foo"};
    auto foo = Foo{"/foo"};
    std::string str = "/foo";
    const char *chr = "/foo";

    EXPECT_TRUE(control == foo);
    EXPECT_TRUE(control == str);
    EXPECT_TRUE(control == chr);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
