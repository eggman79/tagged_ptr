cmake_minimum_required(VERSION 3.1)
include(GNUInstallDirs)


project(flag_ptr LANGUAGES CXX VERSION 0.6.3)

option(FLAG_PTR_BUILD_TESTS "Build flag_ptr tests + enable CTest" ON)
cmake_minimum_required(VERSION 3.18.4)
add_library(flag_ptr INTERFACE)
add_library(flag_ptr::flag_ptr ALIAS flag_ptr)
target_compile_features(flag_ptr INTERFACE cxx_std_17)

target_include_directories(flag_ptr INTERFACE
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
                           "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

list(APPEND headers "${CMAKE_CURRENT_SOURCE_DIR}/include/flag_ptr/flag_ptr.hpp")
target_sources(flag_ptr INTERFACE "$<BUILD_INTERFACE:${headers}>")

#if(MSVC)
#    target_sources(flag_ptr INTERFACE
#                   "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/flag_ptr.natvis>"
#                   "$<INSTALL_INTERFACE:${CMAKE_INSTALL_DATAROOTDIR}/flag_ptr.natvis>")
#endif()




if(${CMAKE_VERSION} VERSION_GREATER "3.2")
    include(CMakePackageConfigHelpers)

    install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/flg_ptr"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

    if(MSVC)
        install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/flag_ptr.natvis"
                DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}")
    endif()



    configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/flag_ptrConfig.cmake.in"
                                  "${CMAKE_CURRENT_BINARY_DIR}/flag_ptrConfig.cmake"
                                  INSTALL_DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/flag_ptr")

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/flag_ptrConfig.cmake"
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/flag_ptr")



    install(TARGETS flag_ptr
            EXPORT flag_ptrTargets)

    install(EXPORT flag_ptrTargets
            NAMESPACE flag_ptr::
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/flag_ptr")



    set(CMAKE_SIZEOF_VOID_P_BACKUP ${CMAKE_SIZEOF_VOID_P})
    unset(CMAKE_SIZEOF_VOID_P)
    write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/flag_ptrConfigVersion.cmake"
                                     COMPATIBILITY SameMajorVersion)
    set(CMAKE_SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P_BACKUP})

    install(FILES "${CMAKE_CURRENT_BINARY_DIR}/flag_ptrConfigVersion.cmake"
            DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/cmake/flag_ptr")
endif()

if (FLAG_PTR_BUILD_TESTS)
    add_subdirectory(3rdparty/googletest)
    enable_testing()
    add_subdirectory(tests)
endif()


