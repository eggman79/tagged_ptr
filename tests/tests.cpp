#include <string>
#include <vector>
#include <gtest/gtest.h>
#include "flag_ptr.h"

using namespace eggman79;

TEST(flag_ptr, set_flag) {
    auto obj = make_flag_ptr<std::string, flags<flag<bool, 1>>>("string");
    obj.set_flag<0>(true);
    EXPECT_TRUE(obj.get_flag<0>());
}

TEST(flag_ptr, get_flag) {
    flag_ptr<std::string, flags<flag<bool, 1>>> obj;
    obj.set_flag<0>(true);
    EXPECT_TRUE(obj.get_flag<0>());
}

TEST(flag_ptr, check_ptr_value) {
    auto obj = make_flag_ptr<std::string, flags<flag<bool, 1>>>("string");
    EXPECT_EQ(*obj, "string");
}

TEST(flag_ptr, bool_operator_return) {
    auto obj = make_flag_ptr<std::string, flags<flag<bool, 1>, flag<bool, 1>>>();
    EXPECT_TRUE(obj);

    flag_ptr<std::string, flags<flag<bool, 1>>> false_obj;
    EXPECT_FALSE(false_obj);
}

TEST(flag_ptr, flag_size) {
    using Flags = flags<flag<bool, 1>>;
    flag_ptr<std::string, flags<flag<bool, 1>>> obj;
    EXPECT_EQ(obj.get_flag_size<0>(), 1);
}

TEST(flag_ptr, flags_size_and_offset) {
    flag_ptr<std::string, flags<flag<uint8_t, 2>, flag<bool, 1>>> obj;
    EXPECT_EQ(obj.get_flag_size<0>(), 2);
    EXPECT_EQ(obj.get_flag_size<1>(), 1);
    EXPECT_EQ(obj.get_flag_offset<1>(), 2);
}

TEST(flag_ptr, flags_size_and_offset_2) {
    flag_ptr<std::string, flags<flag<bool, 1>, flag<bool, 1>, flag<bool, 1>>> obj;
    EXPECT_EQ(obj.get_flag_size<0>(), 1);
    EXPECT_EQ(obj.get_flag_size<1>(), 1);
    EXPECT_EQ(obj.get_flag_size<2>(), 1);
    EXPECT_EQ(obj.get_flag_offset<0>(), 0);
    EXPECT_EQ(obj.get_flag_offset<1>(), 1);
    EXPECT_EQ(obj.get_flag_offset<2>(), 2);
}

TEST(flag_ptr, flags_size) {
    flag_ptr<std::string, flags<flag<uint8_t, 2>, flag<bool, 1>>> obj;
    EXPECT_EQ(obj.get_flags_size(), obj.get_flag_size<0>() + obj.get_flag_size<1>());
}

TEST(flag_ptr, flags_size_2) {
    using StringFlags = flags<flag<uint8_t, 1>, flag<bool, 1>, flag<uint32_t, 1>>;
    auto obj = make_flag_ptr<std::string, StringFlags>("str");
    EXPECT_EQ(obj.get_flags_size(), obj.get_flag_size<0>() + obj.get_flag_size<1>() + obj.get_flag_size<2>());
    EXPECT_EQ(obj.get_flag_offset<2>() + obj.get_flag_size<2>(), obj.get_flag_size<0>() + obj.get_flag_size<1>() + obj.get_flag_size<2>());
}

TEST(flag_ptr, simple_case) {
    using StringFlags = flags<flag<uint8_t, 1>, flag<bool, 1>, flag<uint32_t, 1>>;
    auto obj = make_flag_ptr<std::string, StringFlags>("string");
    EXPECT_TRUE(obj);
    obj.reset();
    EXPECT_FALSE(obj);
    EXPECT_EQ(obj.get_flag_offset<0>(), 0);
    EXPECT_EQ(obj.get_flag_offset<1>(), 1);
    EXPECT_EQ(obj.get_flag_offset<2>(), 2);

    obj.set_flag<0>(1);
    obj.set_flag<1>(false);
    obj.set_flag<2>(1);

    EXPECT_EQ(obj.get_flag<0>(), 1);
    EXPECT_EQ(obj.get_flag<1>(), false);
    EXPECT_EQ(obj.get_flag<2>(), 1);
}

TEST(flag_ptr, struct_case) {
    struct StructFlags { bool own: 1; bool deleted: 1; };
    using Flags = flags<flag<bool, 1>, flag<StructFlags, 2>>;
    auto obj = make_flag_ptr<std::string, Flags>("test");
    obj.set_flag<1>(StructFlags{true, true});
}

TEST(flag_ptr, enum_case) {
    enum class Color{None, Red, Blue, White};
    using Flags = flags<flag<Color, 2>, flag<bool, 1>>;
    flag_ptr<std::string, Flags> obj;
    obj.set_flag<0>(Color::White);
    obj.set_flag<1>(true);

    EXPECT_FALSE(obj);
    obj.reset_only_ptr(new std::string("test"));

    EXPECT_TRUE(obj);
    EXPECT_TRUE(obj.get_flag<1>());
    EXPECT_EQ(obj.get_flag<0>(), Color::White);
    EXPECT_EQ(*obj, "test");
}

TEST(flag_ptr, make_flag_ptr) {
    struct StringFlags {
        bool own : 1;
        bool deleted : 1;
    };
    auto obj = make_flag_ptr<std::string, flags<flag<StringFlags, 2>, flag<bool, 1>>>("string");

    EXPECT_TRUE(obj);
    EXPECT_EQ(*obj, "string");
    EXPECT_EQ(obj.get_flag_size<0>(), 2);
    EXPECT_EQ(obj.get_flag_size<1>(), 1);

    EXPECT_FALSE(obj.get_flag<0>().own);
    EXPECT_FALSE(obj.get_flag<0>().deleted);
    EXPECT_FALSE(obj.get_flag<1>());

    obj.set_flag<0>(StringFlags{true, true});
    obj.set_flag<1>(true);
    EXPECT_TRUE(obj.get_flag<0>().own);
    EXPECT_TRUE(obj.get_flag<0>().deleted);
    EXPECT_TRUE(obj.get_flag<1>());

    obj.reset_only_ptr();
    EXPECT_FALSE(obj);
    EXPECT_TRUE(obj.get_flag<0>().own);
    EXPECT_TRUE(obj.get_flag<0>().deleted);
    EXPECT_TRUE(obj.get_flag<1>());

    obj.reset();
    EXPECT_FALSE(obj.get_flag<0>().own);
    EXPECT_FALSE(obj.get_flag<0>().deleted);
    EXPECT_FALSE(obj.get_flag<1>());
}

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
