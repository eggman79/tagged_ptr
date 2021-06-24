project(flag_ptr_tests)
add_executable(flag_ptr_tests tests.cpp)
target_compile_features(flag_ptr_tests PUBLIC cxx_std_20)

target_link_libraries(flag_ptr_tests PRIVATE gtest)
target_include_directories(flag_ptr_tests PRIVATE ../include)
target_include_directories(flag_ptr_tests PRIVATE gtest)

add_test(flag_ptr_tests flag_ptr_tests)
