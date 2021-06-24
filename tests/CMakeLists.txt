project(flag_ptr_tests)
add_executable(flag_ptr_tests tests.cpp)
set_property(TARGET flag_ptr_tests PROPERTY CXX_STANDARD 20)

target_link_libraries(flag_ptr_tests PRIVATE gtest)
target_include_directories(flag_ptr_tests PRIVATE ../include)
target_include_directories(flag_ptr_tests PRIVATE gtest)

add_test(flag_ptr_tests flag_ptr_tests)
