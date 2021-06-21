project(flag_ptr)
find_package(benchmark REQUIRED)
add_subdirectory(3rdparty/googletest)
enable_testing()
add_subdirectory(tests)

#target_link_libraries(flag_ptr_benchmarks PUBLIC benchmark::benchmark)
#target_compile_features(flag_ptr_benchmarks PUBLIC cxx_std_20)
#target_include_directories(flag_ptr_benchmarks PUBLIC include)
