cmake_minimum_required(VERSION 3.15)
project(cpr VERSION 1.9.0 LANGUAGES CXX)

math(EXPR cpr_VERSION_NUM "${cpr_VERSION_MAJOR} * 0x10000 + ${cpr_VERSION_MINOR} * 0x100 + ${cpr_VERSION_PATCH}" OUTPUT_FORMAT HEXADECIMAL)
configure_file("${cpr_SOURCE_DIR}/cmake/cprver.h.in" "${cpr_BINARY_DIR}/cpr_generated_includes/cpr/cprver.h")

# Only change the folder behaviour if cpr is not a subproject
if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
endif()

# Avoid the dll boilerplate code for windows
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CPR_LIBRARIES cpr CACHE INTERNAL "")

macro(cpr_option OPTION_NAME OPTION_TEXT OPTION_DEFAULT)
    option(${OPTION_NAME} ${OPTION_TEXT} ${OPTION_DEFAULT})
    if(DEFINED ENV{${OPTION_NAME}})
        # Allow overriding the option through an environment variable
        set(${OPTION_NAME} $ENV{${OPTION_NAME}})
    endif()
    if(${OPTION_NAME})
        add_definitions(-D${OPTION_NAME})
    endif()
    message(STATUS "  ${OPTION_NAME}: ${${OPTION_NAME}}")
endmacro()

option(BUILD_SHARED_LIBS "Build libraries as shared libraries" ON)
message(STATUS "C++ Requests CMake Options")
message(STATUS "=======================================================")
cpr_option(CPR_GENERATE_COVERAGE "Set to ON to generate coverage reports." OFF)
cpr_option(CPR_CURL_NOSIGNAL "Set to ON to disable use of signals in libcurl." OFF)
cpr_option(CPR_USE_SYSTEM_GTEST "If ON, this project will look in the system paths for an installed gtest library. If none is found it will use the build in one." OFF)
cpr_option(CPR_FORCE_USE_SYSTEM_CURL "If enabled we will use the curl lib already installed on this system." OFF)
cpr_option(CPR_ENABLE_SSL "Enables or disables the SSL backend. Required to perform HTTPS requests." ON)
cpr_option(CPR_FORCE_OPENSSL_BACKEND "Force to use the OpenSSL backend. If CPR_FORCE_OPENSSL_BACKEND, CPR_FORCE_DARWINSSL_BACKEND, CPR_FORCE_MBEDTLS_BACKEND, and CPR_FORCE_WINSSL_BACKEND are set to to OFF, cpr will try to automatically detect the best available SSL backend (WinSSL - Windows, OpenSSL - Linux, DarwinSSL - Mac ...)." OFF)
cpr_option(CPR_FORCE_WINSSL_BACKEND "Force to use the WinSSL backend. If CPR_FORCE_OPENSSL_BACKEND, CPR_FORCE_DARWINSSL_BACKEND, CPR_FORCE_MBEDTLS_BACKEND, and CPR_FORCE_WINSSL_BACKEND are set to to OFF, cpr will try to automatically detect the best available SSL backend (WinSSL - Windows, OpenSSL - Linux, DarwinSSL - Mac ...)." OFF)
cpr_option(CPR_FORCE_DARWINSSL_BACKEND "Force to use the DarwinSSL backend. If CPR_FORCE_OPENSSL_BACKEND, CPR_FORCE_DARWINSSL_BACKEND, CPR_FORCE_MBEDTLS_BACKEND, and CPR_FORCE_WINSSL_BACKEND are set to to OFF, cpr will try to automatically detect the best available SSL backend (WinSSL - Windows, OpenSSL - Linux, DarwinSSL - Mac ...)." OFF)
cpr_option(CPR_FORCE_MBEDTLS_BACKEND "Force to use the Mbed TLS backend. If CPR_FORCE_OPENSSL_BACKEND, CPR_FORCE_DARWINSSL_BACKEND, CPR_FORCE_MBEDTLS_BACKEND, and CPR_FORCE_WINSSL_BACKEND are set to to OFF, cpr will try to automatically detect the best available SSL backend (WinSSL - Windows, OpenSSL - Linux, DarwinSSL - Mac ...)." OFF)
cpr_option(CPR_ENABLE_LINTING "Set to ON to enable clang linting." OFF)
cpr_option(CPR_ENABLE_CPPCHECK "Set to ON to enable Cppcheck static analysis. Requires CPR_BUILD_TESTS and CPR_BUILD_TESTS_SSL to be OFF to prevent checking google tests source code." OFF)
cpr_option(CPR_BUILD_TESTS "Set to ON to build cpr tests." OFF)
cpr_option(CPR_BUILD_TESTS_SSL "Set to ON to build cpr ssl tests" ${CPR_BUILD_TESTS})
message(STATUS "=======================================================")

include(GNUInstallDirs)
include(FetchContent)
include(cmake/code_coverage.cmake)
include(cmake/sanitizer.cmake)
include(cmake/clear_variable.cmake)

# So CMake can find FindMbedTLS.cmake
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")

# Linting
if(CPR_ENABLE_LINTING)
    include(cmake/clang-tidy.cmake)
endif()

# Cppcheck
if(CPR_ENABLE_CPPCHECK)
    if(CPR_BUILD_TESTS OR CPR_BUILD_TESTS_SSL)
        message(FATAL_ERROR "Cppcheck is incompatible with building tests. Make sure to disable CPR_ENABLE_CPPCHECK or disable tests by setting CPR_BUILD_TESTS and CPR_BUILD_TESTS_SSL to OFF. This is because Cppcheck would try to check the google tests source code and then fail. ")
    endif()
    include(cmake/cppcheck.cmake)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
endif()

# SSL
if(CPR_ENABLE_SSL)
    if(CPR_FORCE_OPENSSL_BACKEND OR CPR_FORCE_WINSSL_BACKEND OR CPR_FORCE_DARWINSSL_BACKEND OR CPR_FORCE_MBEDTLS_BACKEND)
        message(STATUS "Disabled SSL backend auto detect since either CPR_FORCE_OPENSSL_BACKEND, CPR_FORCE_DARWINSSL_BACKEND, CPR_FORCE_MBEDTLS_BACKEND, or CPR_FORCE_WINSSL_BACKEND is enabled.")
        set(DETECT_SSL_BACKEND OFF CACHE INTERNAL "" FORCE)
    else()
        message(STATUS "Automatically detecting SSL backend.")
        set(DETECT_SSL_BACKEND ON CACHE INTERNAL "" FORCE)
    endif()

    if(CPR_FORCE_WINSSL_BACKEND AND (NOT WIN32))
        message(FATAL_ERROR "WinSSL is only available on Windows! Use either OpenSSL (CPR_FORCE_OPENSSL_BACKEND) or DarwinSSL (CPR_FORCE_DARWINSSL_BACKEND) instead.")
    endif()

    if(DETECT_SSL_BACKEND)
        message(STATUS "Detecting SSL backend...")
        if(WIN32)
            message(STATUS "SSL auto detect: Using WinSSL.")
            set(SSL_BACKEND_USED "WinSSL")
        elseif(APPLE)
            message(STATUS "SSL auto detect: Using DarwinSSL.")
            set(CPR_BUILD_TESTS_SSL OFF)
            set(SSL_BACKEND_USED "DarwinSSL")
        else()
            find_package(OpenSSL)
            if(OPENSSL_FOUND)
                message(STATUS "SSL auto detect: Using OpenSSL.")
                set(SSL_BACKEND_USED "OpenSSL")
            else()
                find_package(MbedTLS)
                if(MBEDTLS_FOUND)
                    set(SSL_BACKEND_USED "MbedTLS")
                else()
                    message(FATAL_ERROR "No valid SSL backend found! Please install OpenSSL, Mbed TLS or disable SSL by setting CPR_ENABLE_SSL to OFF.")
                endif()
            endif()
        endif()
    else()
        if(CPR_FORCE_OPENSSL_BACKEND)
            find_package(OpenSSL)
            if(OPENSSL_FOUND)
                message(STATUS "Using OpenSSL.")
                set(SSL_BACKEND_USED "OpenSSL")
            else()
                message(FATAL_ERROR "CPR_FORCE_OPENSSL_BACKEND enabled but we were not able to find OpenSSL!")
            endif()
        elseif(CPR_FORCE_WINSSL_BACKEND)
            message(STATUS "Using WinSSL.")
            set(SSL_BACKEND_USED "WinSSL")
        elseif(CPR_FORCE_DARWINSSL_BACKEND)
            message(STATUS "Using DarwinSSL.")
            set(CPR_BUILD_TESTS_SSL OFF)
            set(SSL_BACKEND_USED "DarwinSSL")
        elseif(CPR_FORCE_MBEDTLS_BACKEND)
            message(STATUS "Using Mbed TLS.")
            set(CPR_BUILD_TESTS_SSL OFF)
            set(SSL_BACKEND_USED "MbedTLS")
        endif()
    endif()
endif()

if(SSL_BACKEND_USED STREQUAL "OpenSSL")
# Fix missing OpenSSL includes for Windows since in 'ssl_ctx.cpp' we include OpenSSL directly
find_package(OpenSSL REQUIRED)
    add_compile_definitions(OPENSSL_BACKEND_USED)
endif()

get_property(isMultiConfig GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (NOT isMultiConfig)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "${ALLOWED_BUILD_TYPES}")
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
    elseif(NOT CMAKE_BUILD_TYPE IN_LIST ALLOWED_BUILD_TYPES)
        message(FATAL_ERROR "Invalid build type: ${CMAKE_BUILD_TYPE}")
    endif()
else ()
    unset(CMAKE_BUILD_TYPE)
    foreach(TYPE ${ALLOWED_BUILD_TYPES})
        if (NOT ${TYPE} IN_LIST CMAKE_CONFIGURATION_TYPES)
            list(APPEND CMAKE_CONFIGURATION_TYPES ${TYPE})
        endif()
    endforeach()
endif()

# Curl configuration
if(CPR_FORCE_USE_SYSTEM_CURL)
    if(CPR_ENABLE_SSL)
        find_package(CURL COMPONENTS HTTP HTTPS)
        if(CURL_FOUND)
            message(STATUS "Curl ${CURL_VERSION_STRING} found on this system.")
            # To be able to load certificates under Windows when using OpenSSL:
            if(CMAKE_USE_OPENSSL AND WIN32 AND (NOT (CURL_VERSION_STRING VERSION_GREATER_EQUAL "7.71.0")))
                message(FATAL_ERROR "Your system curl version (${CURL_VERSION_STRING}) is too old to support OpenSSL on Windows which requires curl >= 7.71.0. Update your curl version, use WinSSL, disable SSL or use the build in version of curl.")
            endif()
        else()
            find_package(CURL COMPONENTS HTTP)
            if(CURL_FOUND)
                message(FATAL_ERROR "Curl found on this system but WITHOUT HTTPS/SSL support. Either disable SSL by setting CPR_ENABLE_SSL to OFF or use the build in version of curl by setting CPR_FORCE_USE_SYSTEM_CURL to OFF.")
            else()
                message(FATAL_ERROR "Curl not found on this system. To use the build in version set CPR_FORCE_USE_SYSTEM_CURL to OFF.")
            endif()
        endif()
    else()
        find_package(CURL COMPONENTS HTTP)
        if(CURL_FOUND)
            message(STATUS "Curl found on this system.")
        else()
            message(FATAL_ERROR "Curl not found on this system. To use the build in version set CPR_FORCE_USE_SYSTEM_CURL to OFF.")
        endif()
    endif()
else()
    message(STATUS "Configuring build in curl...")

    # ZLIB is optional for curl
    # to disable it:
    # * from command line:
    #     -DCURL_ZLIB=OFF
    # * from CMake script:
    #     SET(CURL_ZLIB OFF CACHE STRING "" FORCE)
    if (CURL_ZLIB OR CURL_ZLIB STREQUAL AUTO OR NOT DEFINED CACHE{CURL_ZLIB})
        include(cmake/zlib_external.cmake)
    endif()

    # We only need HTTP (and HTTPS) support:
    set(HTTP_ONLY ON CACHE INTERNAL "" FORCE)
    set(BUILD_CURL_EXE OFF CACHE INTERNAL "" FORCE)
    set(BUILD_TESTING OFF)

    if (CPR_ENABLE_SSL)
        set(SSL_ENABLED ON CACHE INTERNAL "" FORCE)
        if(ANDROID)
            set(CURL_CA_PATH "/system/etc/security/cacerts" CACHE INTERNAL "")
        else()
            set(CURL_CA_PATH "auto" CACHE INTERNAL "")
        endif()

        if(NOT DEFINED CURL_CA_BUNDLE)
            set(CURL_CA_BUNDLE "auto" CACHE INTERNAL "")
        endif()

        if(SSL_BACKEND_USED STREQUAL "WinSSL")
            set(CMAKE_USE_SCHANNEL ON CACHE INTERNAL "" FORCE)
        endif()

        if(SSL_BACKEND_USED STREQUAL "OpenSSL")
            set(CMAKE_USE_OPENSSL ON CACHE INTERNAL "" FORCE)
        endif()

        if(SSL_BACKEND_USED STREQUAL "DarwinSSL")
            set(CMAKE_USE_SECTRANSP ON CACHE INTERNAL "" FORCE)
        endif()

        if(SSL_BACKEND_USED STREQUAL "MbedTLS")
            set(CMAKE_USE_MBEDTLS ON CACHE INTERNAL "" FORCE)
        endif()

        message(STATUS "Enabled curl SSL")
    else()
        set(SSL_ENABLED OFF CACHE INTERNAL "" FORCE)
        set(CURL_CA_PATH "none" CACHE INTERNAL "" FORCE)
        set(CMAKE_USE_SCHANNEL OFF CACHE INTERNAL "" FORCE)
        set(CMAKE_USE_OPENSSL OFF CACHE INTERNAL "" FORCE)
        set(CMAKE_USE_SECTRANSP OFF CACHE INTERNAL "" FORCE)
        set(CMAKE_USE_MBEDTLS OFF CACHE INTERNAL "" FORCE)
        message(STATUS "Disabled curl SSL")
    endif()
    # Disable linting for curl
    clear_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)

    FetchContent_Declare(curl
                         URL                    https://github.com/curl/curl/releases/download/curl-7_80_0/curl-7.80.0.tar.xz
                         URL_HASH               SHA256=a132bd93188b938771135ac7c1f3ac1d3ce507c1fcbef8c471397639214ae2ab # the file hash for curl-7.80.0.tar.xz
                         USES_TERMINAL_DOWNLOAD TRUE)   # <---- This is needed only for Ninja to show download progress
    FetchContent_MakeAvailable(curl)

    restore_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)

    # Group under the "external" project folder in IDEs such as Visual Studio.
    if(BUILD_CURL_EXE)
        set_property(TARGET curl PROPERTY FOLDER "external")
    endif()
        
    set_property(TARGET libcurl PROPERTY FOLDER "external")
endif()

# GTest configuration
if(CPR_BUILD_TESTS)
    if(CPR_USE_SYSTEM_GTEST)
        find_package(GTest)
    endif()
    if(NOT CPR_USE_SYSTEM_GTEST OR NOT GTEST_FOUND)
        message(STATUS "Not using system gtest, using built-in googletest project instead.")
        if(MSVC)
            # By default, GTest compiles on Windows in CRT static linkage mode. We use this
            # variable to force it into using the CRT in dynamic linkage (DLL), just as CPR
            # does.
            set(gtest_force_shared_crt ON CACHE BOOL "Force gtest to use the shared c runtime")
        endif()

        # Disable linting for google test
        clear_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)

        FetchContent_Declare(googletest
                             URL                    https://github.com/google/googletest/archive/release-1.11.0.tar.gz
                             URL_HASH               SHA256=b4870bf121ff7795ba20d20bcdd8627b8e088f2d1dab299a031c1034eddc93d5 # the file hash for release-1.11.0.tar.gz
                             USES_TERMINAL_DOWNLOAD TRUE)   # <---- This is needed only for Ninja to show download progress
        FetchContent_MakeAvailable(googletest)

        restore_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)
        
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
if(CPR_BUILD_TESTS)
    message(STATUS "Building mongoose project for test support.")

    if(CPR_BUILD_TESTS_SSL)
        if(NOT CPR_ENABLE_SSL)
            message(FATAL_ERROR "OpenSSL is required to build SSL test but CPR_ENABLE_SSL is disabled. Either set CPR_ENABLE_SSL to ON or disable CPR_BUILD_TESTS_SSL.")
        endif()

        if(NOT(SSL_BACKEND_USED STREQUAL "OpenSSL"))
            message(FATAL_ERROR "OpenSSL is required for SSL test, but it seams like OpenSSL is not being used as SSL backend. Either set CPR_BUILD_TESTS_SSL to OFF or set CPR_FORCE_OPENSSL_BACKEND to ON and try again.")
        endif()

        set(ENABLE_SSL_TESTS ON CACHE INTERNAL "")
    else()
        set(ENABLE_SSL_TESTS OFF CACHE INTERNAL "")
    endif()

    # Disable linting for mongoose
    clear_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)

    FetchContent_Declare(mongoose 
                         URL                    https://github.com/cesanta/mongoose/archive/6.18.tar.gz
                         URL_HASH               SHA256=f5c10346abc9c72f7cac7885d853ca064fb09aad57580433941a8fd7a3543769 # the hash for 6.18.tar.gz
                         USES_TERMINAL_DOWNLOAD TRUE)   # <---- This is needed only for Ninja to show download progress
    # We can not use FetchContent_MakeAvailable, since we need to patch mongoose to use CMake
    if (NOT mongoose_POPULATED)
        FetchContent_POPULATE(mongoose)

        file(INSTALL cmake/mongoose.CMakeLists.txt DESTINATION ${mongoose_SOURCE_DIR})
        file(RENAME ${mongoose_SOURCE_DIR}/mongoose.CMakeLists.txt ${mongoose_SOURCE_DIR}/CMakeLists.txt)
        add_subdirectory(${mongoose_SOURCE_DIR} ${mongoose_BINARY_DIR})

    endif()
    # Group under the "external" project folder in IDEs such as Visual Studio.
    set_property(TARGET mongoose PROPERTY FOLDER "external")
    restore_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)
endif()


add_subdirectory(cpr)
add_subdirectory(include)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND CPR_BUILD_TESTS)
    # Disable linting for test since they are currently not up to the standard
    clear_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)
    enable_testing()
    add_subdirectory(test)
    restore_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)
endif()
