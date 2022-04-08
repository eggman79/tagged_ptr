cmake_minimum_required(VERSION 3.1)
include(GNUInstallDirs)


project(tagged_ptr LANGUAGES CXX VERSION 0.0.1)

option(FLAG_PTR_BUILD_TESTS "Build tagged_ptr tests + enable CTest" OFF)

cmake_minimum_required(VERSION 3.16.0)
add_library(tagged_ptr INTERFACE)
add_library(tagged_ptr::tagged_ptr ALIAS tagged_ptr)
target_compile_features(tagged_ptr INTERFACE cxx_std_17)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)

target_include_directories(tagged_ptr INTERFACE
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

list(APPEND headers "${CMAKE_CURRENT_SOURCE_DIR}/include/tagged_ptr/tagged_ptr.hpp")
target_sources(tagged_ptr INTERFACE "$<BUILD_INTERFACE:${headers}>")

#if(MSVC)
#    target_sources(tagged_ptr INTERFACE
#                   "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/tagged_ptr.natvis>"
#                   "$<INSTALL_INTERFACE:${CMAKE_INSTALL_DATAROOTDIR}/tagged_ptr.natvis>")
#endif()




if(${CMAKE_VERSION} VERSION_GREATER "3.2")
    include(CMakePackageConfigHelpers)

    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/tagged_ptr"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

    if(MSVC)
        install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/tagged_ptr.natvis"
                DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}")
    endif()

    configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/tagged_ptrConfig.cmake.in"
                                  "${CMAKE_CURRENT_BINARY_DIR}/tagged_ptrConfig.cmake"
                                  INSTALL_DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/tagged_ptr")

                                  install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tagged_ptrConfig.cmake" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/tagged_ptr")

                              install(TARGETS tagged_ptr EXPORT tagged_ptrTargets)

    install(EXPORT tagged_ptrTargets NAMESPACE tagged_ptr:: DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/tagged_ptr")


    set(CMAKE_SIZEOF_VOID_P_BACKUP ${CMAKE_SIZEOF_VOID_P})
    unset(CMAKE_SIZEOF_VOID_P)
    write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/tagged_ptrConfigVersion.cmake" COMPATIBILITY SameMajorVersion)
    set(CMAKE_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P_BACKUP})

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/tagged_ptrConfigVersion.cmake" DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/tagged_ptr")
endif()

if (TAGGED_PTR_BUILD_TESTS)
    add_subdirectory(3rdparty/googletest)
    enable_testing()
    add_subdirectory(tests)
endif()


