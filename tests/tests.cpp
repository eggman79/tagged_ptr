#include <string>
#include <vector>
#include <gtest/gtest.h>
#include "flag_ptr.h"

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
    flag_ptr<std::string, StringFlags> obj("str");
    EXPECT_EQ(obj.get_flags_size(), obj.get_flag_size<0>() + obj.get_flag_size<1>() + obj.get_flag_size<2>());
    EXPECT_EQ(obj.get_flag_offset<2>() + obj.get_flag_size<2>(), obj.get_flag_size<0>() + obj.get_flag_size<1>() + obj.get_flag_size<2>());
}

TEST(flag_ptr, simple_case) {
    using StringFlags = flags<flag<uint8_t, 1>, flag<bool, 1>, flag<uint32_t, 1>>;
    flag_ptr<std::string, StringFlags> obj("string");
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
    flag_ptr<std::string, Flags> obj("test");
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

int main(int argc, char* argv[]) {
    ::testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}
