cmake_minimum_required(VERSION 3.15)

macro(set_cache_variable VAR_NAME VAR_DESCRIPTION)
    set(${VAR_NAME} ${${VAR_NAME}} CACHE INTERNAL ${VAR_DESCRIPTION})
    message(STATUS "Set ${VAR_NAME} to ${${VAR_NAME}}.")
endmacro()


# Code coverage

if(BUILD_CPR_TESTS AND GENERATE_COVERAGE)
    set(CMAKE_BUILD_TYPE COVERAGE CACHE INTERNAL "Coverage enabled build")
    message(STATUS "Enabling gcov support")
    if(NOT "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(COVERAGE_FLAG "--coverage")
    endif()
    set(CMAKE_CXX_FLAGS_COVERAGE
        "-g -O0 ${COVERAGE_FLAG} -fprofile-arcs -ftest-coverage"
        CACHE STRING "Flags used by the C++ compiler during coverage builds."
        FORCE)
    set(CMAKE_C_FLAGS_COVERAGE
        "-g -O0 ${COVERAGE_FLAG} -fprofile-arcs -ftest-coverage"
        CACHE STRING "Flags used by the C compiler during coverage builds."
        FORCE)
    set(CMAKE_EXE_LINKER_FLAGS_COVERAGE
        ""
        CACHE STRING "Flags used for linking binaries during coverage builds."
        FORCE)
    set(CMAKE_SHARED_LINKER_FLAGS_COVERAGE
        ""
        CACHE STRING "Flags used by the shared libraries linker during coverage builds."
        FORCE)
    mark_as_advanced(
        CMAKE_CXX_FLAGS_COVERAGE
        CMAKE_C_FLAGS_COVERAGE
        CMAKE_EXE_LINKER_FLAGS_COVERAGE
        CMAKE_SHARED_LINKER_FLAGS_COVERAGE)
endif()

include(FetchContent)

# Curl configuration

if(USE_SYSTEM_CURL)
    find_package(CURL)
endif()
if(NOT USE_SYSTEM_CURL OR NOT CURL_FOUND)
    message(STATUS "Not using system Curl, using built-in curl project instead.")
    option(BUILD_TESTING "Set to ON to build cURL tests." OFF)
    option(BUILD_CURL_EXE "Set to ON to build cURL executable." OFF)
    FetchContent_Declare(curl
                          GIT_REPOSITORY https://github.com/curl/curl.git
                          GIT_TAG        curl-7_69_1)
    FetchContent_MakeAvailable(curl)

    add_library(curl_int INTERFACE)
    target_link_libraries(curl_int INTERFACE libcurl)
    target_include_directories(curl_int INTERFACE ${curl_SOURCE_DIR}/include ${curl_BINARY_DIR}/include/curl)
    add_library(CURL::libcurl ALIAS curl_int)

        
    # Group under the "external" project folder in IDEs such as Visual Studio.
    if(BUILD_CURL_EXE)
        set_property(TARGET curl PROPERTY FOLDER "external")
    endif()
        
    set_property(TARGET libcurl PROPERTY FOLDER "external")
endif()


# SSL configuration
if(CMAKE_USE_OPENSSL)
    find_package(OpenSSL REQUIRED)

    message(STATUS "using system OpenSSL library.")

    include_directories(${OPENSSL_INCLUDE_DIR})
    set(SSL_LIBRARIES ${OPENSSL_LIBRARIES})
endif()

# GTest configuration

if(BUILD_CPR_TESTS)
    if(USE_SYSTEM_GTEST)
        find_package(GTest)
    endif()
    if(NOT USE_SYSTEM_GTEST OR NOT GTEST_FOUND)
        message(STATUS "Not using system gtest, using built-in googletest project instead.")
        if(MSVC)
            # By default, GTest compiles on Windows in CRT static linkage mode. We use this
            # variable to force it into using the CRT in dynamic linkage (DLL), just as CPR
            # does.
            set(gtest_force_shared_crt ON CACHE BOOL "Force gtest to use the shared c runtime")
        endif()
        FetchContent_Declare(googletest
                                    GIT_REPOSITORY https://github.com/google/googletest.git
                                    GIT_TAG release-1.10.0)
        FetchContent_MakeAvailable(googletest)

        
        add_library(gtest_int INTERFACE)
        target_link_libraries(gtest_int INTERFACE gtest)
        target_include_directories(gtest_int INTERFACE ${googletest_SOURCE_DIR}/include)

        add_library(GTest::GTest ALIAS gtest_int)
       
        # Group under the "tests/gtest" project folder in IDEs such as Visual Studio.
    set_property(TARGET gtest PROPERTY FOLDER "tests/gtest")
    set_property(TARGET gtest_main PROPERTY FOLDER "tests/gtest")
    endif()
endif()


# Mongoose configuration

if(BUILD_CPR_TESTS)
    message(STATUS "Building mongoose project for test support.")
    FetchContent_Declare(mongoose GIT_REPOSITORY https://github.com/whoshuu/mongoose.git)
    FetchContent_MakeAvailable(mongoose)

    target_include_directories(mongoose INTERFACE ${mongoose_SOURCE_DIR})

    # Group under the "external" project folder in IDEs such as Visual Studio.
    set_property(TARGET mongoose PROPERTY FOLDER "external")
endif()
