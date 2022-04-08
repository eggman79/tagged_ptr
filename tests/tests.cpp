#include <string>
#include <vector>
#include <gtest/gtest.h>
#include "tagged_ptr/tagged_ptr.hpp"

using namespace eggman79;

TEST(tagged_ptr, set_flag) {
    auto obj = make_tagged_ptr<std::string, flags<flag<bool, 1>>>("string");
    obj.set_flag<0>(true);
    EXPECT_TRUE(obj.get_flag<0>());
}

TEST(tagged_ptr, get_flag) {
    tagged_ptr<std::string, flags<flag<bool, 1>>> obj;
    obj.set_flag<0>(true);
    EXPECT_TRUE(obj.get_flag<0>());
}

TEST(tagged_ptr, check_ptr_value) {
    auto obj = make_tagged_ptr<std::string, flags<flag<bool, 1>>>("string");
    obj.set_flag<0>(true);
    EXPECT_EQ(*obj, "string");
}

TEST(tagged_ptr, bool_operator_return) {
    auto obj = make_tagged_ptr<std::string, flags<flag<bool, 1>, flag<bool, 1>>>();
    EXPECT_TRUE(obj);
    obj.reset();
    EXPECT_FALSE(obj);

    tagged_ptr<std::string, flags<flag<bool, 1>>> false_obj;
    EXPECT_FALSE(false_obj);
}

TEST(tagged_ptr, reset_only_ptr) {
    auto obj = make_tagged_ptr<std::string, flags<flag<bool, 1>, flag<bool, 1>>>("str1");
    EXPECT_EQ(*obj, "str1");
    EXPECT_TRUE(obj);
    obj.set_flag<0>(true);
    EXPECT_TRUE(obj.get_flag<0>());

    obj.reset_only_ptr();
    EXPECT_TRUE(obj.get_flag<0>());
    EXPECT_FALSE(obj);

    obj.reset_only_ptr(new std::string("str2"));
    EXPECT_TRUE(obj.get_flag<0>());
    EXPECT_EQ(*obj, "str2");
}

TEST(tagged_ptr, reset) {
    auto obj = make_tagged_ptr<std::string, flags<flag<bool, 1>>>("str");
    EXPECT_TRUE(obj);
    EXPECT_EQ(*obj, "str");
    obj.set_flag<0>(true);
    EXPECT_TRUE(obj.get_flag<0>());
    obj.reset();
    EXPECT_FALSE(obj);
    EXPECT_FALSE(obj.get_flag<0>());
}

TEST(tagged_ptr, struct_case) {
    struct StructFlags { bool own: 1; bool deleted: 1; };
    using Flags = flags<flag<bool, 1>, flag<StructFlags, 2>>;
    auto obj = make_tagged_ptr<std::string, Flags>("test");
    obj.set_flag<1>(StructFlags{true, true});
    EXPECT_TRUE(obj);
    EXPECT_TRUE(obj.get_flag<1>().own);
    EXPECT_TRUE(obj.get_flag<1>().deleted);
    EXPECT_EQ(*obj, "test");
}

TEST(tagged_ptr, enum_case) {
    enum class Color{None, Red, Blue, White};
    using Flags = flags<flag<Color, 2>, flag<bool, 1>>;
    tagged_ptr<std::string, Flags> obj;
    obj.set_flag<0>(Color::White);
    obj.set_flag<1>(true);

    EXPECT_FALSE(obj);
    obj.reset_only_ptr(new std::string("test"));

    EXPECT_TRUE(obj);
    EXPECT_TRUE(obj.get_flag<1>());
    EXPECT_EQ(obj.get_flag<0>(), Color::White);
    EXPECT_EQ(*obj, "test");
}

TEST(tagged_ptr, set_ptr_value) {
    auto str = make_tagged_ptr<std::string, flags<flag<bool, 1>>>("string");
    *str = "test";
    EXPECT_EQ(*str, "test");
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
