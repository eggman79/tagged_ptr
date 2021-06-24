project(flag_ptr LANGUAGES CXX)
cmake_minimum_required(VERSION 3.18.4)

option(FLAG_PTR_BUILD_TESTS "Build flag_ptr tests + enable CTest" ON)

add_library(flag_ptr INTERFACE)
add_library(flag_ptr::flag_ptr ALIAS flag_ptr)

target_compile_features(flag_ptr INTERFACE cxx_std_20)
target_include_directories(flag_ptr INTERFACE
 $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
 $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>)

install(FILES include/flag_ptr/flag_ptr.hpp DESTINATION include/flag_ptr)

if (FLAG_PTR_BUILD_TESTS)
    add_subdirectory(3rdparty/googletest)
    enable_testing()
    add_subdirectory(tests)
endif()
