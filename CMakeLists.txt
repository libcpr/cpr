cmake_minimum_required(VERSION 3.15)
project(cpr VERSION 1.12.0 LANGUAGES CXX)

math(EXPR cpr_VERSION_NUM "${cpr_VERSION_MAJOR} * 0x10000 + ${cpr_VERSION_MINOR} * 0x100 + ${cpr_VERSION_PATCH}" OUTPUT_FORMAT HEXADECIMAL)
configure_file("${cpr_SOURCE_DIR}/cmake/cprver.h.in" "${cpr_BINARY_DIR}/cpr_generated_includes/cpr/cprver.h")

# Only change the folder behavior if cpr is not a subproject
if(${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME})
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")
    set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
    set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # generate compile_commands.json to be used by other tools (e.g. vs code)
else()
    # Check required c++ standard of parent project
    if(CMAKE_CXX_STANDARD)
        set(PARENT_CXX_STANDARD ${CMAKE_CXX_STANDARD})
        message(STATUS "CXX standard of parent project: ${PARENT_CXX_STANDARD}")
    endif()
endif()

# Avoid the dll boilerplate code for windows
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if (PARENT_CXX_STANDARD)
    # Don't set CMAKE_CXX_STANDARD if it is already set by parent project
    if (PARENT_CXX_STANDARD LESS 17)
        message(FATAL_ERROR "cpr ${cpr_VERSION} does not support ${PARENT_CXX_STANDARD}. Please use cpr <= 1.9.x")
    endif()
else()
    # Set standard version if not already set by potential parent project
    set(CMAKE_CXX_STANDARD 17)
endif()

message(STATUS "CXX standard: ${CMAKE_CXX_STANDARD}")
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

message(STATUS "C++ Requests CMake Options")
message(STATUS "=======================================================")
cpr_option(CPR_GENERATE_COVERAGE "Set to ON to generate coverage reports." OFF)
cpr_option(CPR_CURL_NOSIGNAL "Set to ON to disable use of signals in libcurl." OFF)
cpr_option(CURL_VERBOSE_LOGGING "Curl verbose logging during building curl" OFF)
cpr_option(CPR_USE_SYSTEM_GTEST "If ON, this project will look in the system paths for an installed gtest library. If none is found it will use the built-in one." OFF)
cpr_option(CPR_USE_SYSTEM_CURL "If enabled we will use the curl lib already installed on this system." OFF)
cpr_option(CPR_USE_SYSTEM_LIB_PSL "Since curl 8.13 it depends on libpsl. If enabled we will use the psl lib already installed on this system. Else meson is required as build dependency." ${CPR_USE_SYSTEM_CURL})
cpr_option(CPR_ENABLE_CURL_HTTP_ONLY "If enabled we will only use the HTTP/HTTPS protocols from CURL. If disabled, all the CURL protocols are enabled. This is useful if your project uses libcurl and you need support for other CURL features e.g. sending emails." ON)
cpr_option(CPR_ENABLE_SSL "Enables or disables the SSL backend. Required to perform HTTPS requests." ON)
cpr_option(CPR_FORCE_OPENSSL_BACKEND "Force to use the OpenSSL backend. If CPR_FORCE_OPENSSL_BACKEND, CPR_FORCE_DARWINSSL_BACKEND, CPR_FORCE_MBEDTLS_BACKEND, and CPR_FORCE_WINSSL_BACKEND are set to to OFF, cpr will try to automatically detect the best available SSL backend (WinSSL - Windows, OpenSSL - Linux, DarwinSSL - Mac ...)." OFF)
cpr_option(CPR_FORCE_WINSSL_BACKEND "Force to use the WinSSL backend. If CPR_FORCE_OPENSSL_BACKEND, CPR_FORCE_DARWINSSL_BACKEND, CPR_FORCE_MBEDTLS_BACKEND, and CPR_FORCE_WINSSL_BACKEND are set to to OFF, cpr will try to automatically detect the best available SSL backend (WinSSL - Windows, OpenSSL - Linux, DarwinSSL - Mac ...)." OFF)
cpr_option(CPR_FORCE_DARWINSSL_BACKEND "Force to use the DarwinSSL backend. If CPR_FORCE_OPENSSL_BACKEND, CPR_FORCE_DARWINSSL_BACKEND, CPR_FORCE_MBEDTLS_BACKEND, and CPR_FORCE_WINSSL_BACKEND are set to to OFF, cpr will try to automatically detect the best available SSL backend (WinSSL - Windows, OpenSSL - Linux, DarwinSSL - Mac ...)." OFF)
cpr_option(CPR_FORCE_MBEDTLS_BACKEND "Force to use the Mbed TLS backend. If CPR_FORCE_OPENSSL_BACKEND, CPR_FORCE_DARWINSSL_BACKEND, CPR_FORCE_MBEDTLS_BACKEND, and CPR_FORCE_WINSSL_BACKEND are set to to OFF, cpr will try to automatically detect the best available SSL backend (WinSSL - Windows, OpenSSL - Linux, DarwinSSL - Mac ...)." OFF)
cpr_option(CPR_ENABLE_LINTING "Set to ON to enable clang linting." OFF)
cpr_option(CPR_ENABLE_CPPCHECK "Set to ON to enable Cppcheck static analysis. Requires CPR_BUILD_TESTS and CPR_BUILD_TESTS_SSL to be OFF to prevent checking google tests source code." OFF)
cpr_option(CPR_BUILD_TESTS "Set to ON to build cpr tests." OFF)
cpr_option(CPR_BUILD_TESTS_SSL "Set to ON to build cpr ssl tests" ${CPR_BUILD_TESTS})
cpr_option(CPR_BUILD_TESTS_PROXY "Set to ON to build proxy tests. They fail in case there is no valid proxy server available in proxy_tests.cpp" OFF)
cpr_option(CPR_BUILD_VERSION_OUTPUT_ONLY "Set to ON to only export the version into 'build/version.txt' and exit" OFF)
cpr_option(CPR_SKIP_CA_BUNDLE_SEARCH "Skip searching for Certificate Authority certs. Turn ON for systems like iOS where file access is restricted and prevents https from working." OFF)
cpr_option(CPR_USE_BOOST_FILESYSTEM "Set to ON to use the Boost.Filesystem library. This is useful, on, e.g., Apple platforms, where std::filesystem may not always be available when targeting older OS versions." OFF)
cpr_option(CPR_DEBUG_SANITIZER_FLAG_THREAD "Enables the ThreadSanitizer for debug builds." OFF)
cpr_option(CPR_DEBUG_SANITIZER_FLAG_ADDR "Enables the AddressSanitizer for debug builds." OFF)
cpr_option(CPR_DEBUG_SANITIZER_FLAG_LEAK "Enables the LeakSanitizer for debug builds." OFF)
cpr_option(CPR_DEBUG_SANITIZER_FLAG_UB "Enables the UndefinedBehaviorSanitizer for debug builds." OFF)
cpr_option(CPR_DEBUG_SANITIZER_FLAG_ALL "Enables all sanitizers for debug builds except the ThreadSanitizer since it is incompatible with the other sanitizers." OFF)
message(STATUS "=======================================================")

if (MSVC)
    if (BUILD_SHARED_LIBS)
        message(STATUS "Build windows dynamic libs.")
    else()
        # Add this to build windows pure static library.
        message(STATUS "Build windows static libs.")
    endif()
endif()

# Save the project version as txt file for deb and NuGet builds
if(CPR_BUILD_VERSION_OUTPUT_ONLY)
    message(STATUS "Printing version and exiting...")
    file(WRITE "${CMAKE_BINARY_DIR}/version.txt" "${PROJECT_VERSION}")
    return()
endif()

if (CPR_FORCE_USE_SYSTEM_CURL)
    message(WARNING "The variable CPR_FORCE_USE_SYSTEM_CURL is deprecated, please use CPR_USE_SYSTEM_CURL instead")
    set(CPR_USE_SYSTEM_CURL ${CPR_FORCE_USE_SYSTEM_CURL})
endif()

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

# Curl configuration
if(CPR_USE_SYSTEM_CURL)
    if(CPR_ENABLE_SSL)
        find_package(CURL COMPONENTS HTTP HTTPS)
        if(CURL_FOUND)
            message(STATUS "Curl ${CURL_VERSION_STRING} found on this system.")

            # To be able to load certificates under Windows when using OpenSSL:
            if(CMAKE_USE_OPENSSL AND WIN32 AND (NOT (CURL_VERSION_STRING VERSION_GREATER_EQUAL "7.71.0")))
                message(FATAL_ERROR "Your system curl version (${CURL_VERSION_STRING}) is too old to support OpenSSL on Windows which requires curl >= 7.71.0. Update your curl version, use WinSSL, disable SSL or use the built-in version of curl.")
            endif()
        else()
            find_package(CURL COMPONENTS HTTP)
            if(CURL_FOUND)
                message(FATAL_ERROR "Curl found on this system but WITHOUT HTTPS/SSL support. Either disable SSL by setting CPR_ENABLE_SSL to OFF or use the built-in version of curl by setting CPR_USE_SYSTEM_CURL to OFF.")
            else()
                message(FATAL_ERROR "Curl not found on this system. To use the built-in version set CPR_USE_SYSTEM_CURL to OFF.")
            endif()
        endif()
    else()
        find_package(CURL COMPONENTS HTTP)
        if(CURL_FOUND)
            message(STATUS "Curl found on this system.")
        else()
            message(FATAL_ERROR "Curl not found on this system. To use the built-in version set CPR_USE_SYSTEM_CURL to OFF.")
        endif()
    endif()

    # Check for the minimum supported curl version 
    if(NOT (CURL_VERSION_STRING VERSION_GREATER_EQUAL "7.64.0"))
        message(FATAL_ERROR "Your system curl version (${CURL_VERSION_STRING}) is too old! curl >= 7.64.0 is required. Update your curl version, or use the build in curl version e.g. via `cmake .. -DCPR_USE_SYSTEM_CURL=OFF` during CMake configure.")
    endif()
else()
    message(STATUS "Configuring built-in curl...")

    # ZLIB is optional for curl
    # to disable it:
    # * from command line:
    #     -DCURL_ZLIB=OFF
    # * from CMake script:
    if (CURL_ZLIB OR CURL_ZLIB STREQUAL AUTO OR NOT DEFINED CACHE{CURL_ZLIB})
        include(cmake/zlib_external.cmake)
    endif()

    if (CPR_ENABLE_CURL_HTTP_ONLY)
        # We only need HTTP (and HTTPS) support:
        set(HTTP_ONLY ON CACHE INTERNAL "" FORCE)
    endif()
    set(BUILD_CURL_EXE OFF CACHE INTERNAL "" FORCE)
    set(BUILD_TESTING OFF)

    if (CURL_VERBOSE_LOGGING)
        message(STATUS "Enabled curl debug features")
        set(ENABLE_DEBUG ON CACHE INTERNAL "" FORCE)
    endif()

    if (CPR_ENABLE_SSL)
        set(CURL_ENABLE_SSL ON CACHE INTERNAL "" FORCE)
        if(ANDROID)
            set(CURL_CA_PATH "/system/etc/security/cacerts" CACHE INTERNAL "")
        elseif(CPR_SKIP_CA_BUNDLE_SEARCH)
            set(CURL_CA_PATH "none" CACHE INTERNAL "")
        else()
            set(CURL_CA_PATH "auto" CACHE INTERNAL "")
        endif()

        if(CPR_SKIP_CA_BUNDLE_SEARCH)
            set(CURL_CA_BUNDLE "none" CACHE INTERNAL "")
        elseif(NOT DEFINED CURL_CA_BUNDLE)
            set(CURL_CA_BUNDLE "auto" CACHE INTERNAL "")
        endif()

        if(SSL_BACKEND_USED STREQUAL "WinSSL")
            set(CURL_USE_SCHANNEL ON CACHE INTERNAL "" FORCE)
            set(CURL_WINDOWS_SSPI ON CACHE INTERNAL "" FORCE)
        endif()

        if(SSL_BACKEND_USED STREQUAL "OpenSSL")
            set(CURL_USE_OPENSSL ON CACHE INTERNAL "" FORCE)
        endif()

        if(SSL_BACKEND_USED STREQUAL "DarwinSSL")
            set(CURL_USE_SECTRANSP ON CACHE INTERNAL "" FORCE)
        endif()

        if(SSL_BACKEND_USED STREQUAL "MbedTLS")
            set(CURL_USE_MBEDTLS ON CACHE INTERNAL "" FORCE)
        endif()

        message(STATUS "Enabled curl SSL")
    else()
        set(CURL_ENABLE_SSL OFF CACHE INTERNAL "" FORCE)

        set(CURL_CA_PATH "none" CACHE INTERNAL "" FORCE)
        set(CURL_USE_SCHANNEL OFF CACHE INTERNAL "" FORCE)
        set(CURL_WINDOWS_SSPI OFF CACHE INTERNAL "" FORCE)
        set(CURL_USE_OPENSSL OFF CACHE INTERNAL "" FORCE)
        set(CURL_USE_SECTRANSP OFF CACHE INTERNAL "" FORCE)
        set(CURL_USE_MBEDTLS OFF CACHE INTERNAL "" FORCE)
        message(STATUS "Disabled curl SSL")
    endif()
    # Disable linting for curl
    clear_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)

    if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
        cmake_policy(SET CMP0135 NEW)
    endif()

    # Since curl 8.13, curl depends on lib psl
    if(NOT ${CPR_USE_SYSTEM_LIB_PSL})
        include(libpsl)
    endif()

    FetchContent_Declare(curl URL https://github.com/curl/curl/releases/download/curl-8_13_0/curl-8.13.0.tar.xz
                              URL_HASH SHA256=4a093979a3c2d02de2fbc00549a32771007f2e78032c6faa5ecd2f7a9e152025) # the file hash for curl-8.13.0.tar.xz
    FetchContent_MakeAvailable(curl)

    restore_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)
endif()

# Depending on which version of libcurl we are using the CMake target is called differently
if(TARGET libcurl)
    # Old curl CMake target name
    set(CURL_LIB libcurl)
else()
    # New curl CMake target name
    set(CURL_LIB CURL::libcurl)
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
                             URL                    https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz
                             URL_HASH               SHA256=8ad598c73ad796e0d8280b082cebd82a630d73e73cd3c70057938a6501bba5d7 # the file hash for release-1.14.0.tar.gz
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
                         URL                    https://github.com/cesanta/mongoose/archive/7.7.tar.gz
                         URL_HASH               SHA256=4e5733dae31c3a81156af63ca9aa3a6b9b736547f21f23c3ab2f8e3f1ecc16c0 # the hash for 7.7.tar.gz
                         USES_TERMINAL_DOWNLOAD TRUE)   # <---- This is needed only for Ninja to show download progress
    # We can not use FetchContent_MakeAvailable, since we need to patch mongoose to use CMake
    if (NOT mongoose_POPULATED)
        FetchContent_MakeAvailable(mongoose)

        file(INSTALL cmake/mongoose.CMakeLists.txt DESTINATION ${mongoose_SOURCE_DIR})
        file(RENAME ${mongoose_SOURCE_DIR}/mongoose.CMakeLists.txt ${mongoose_SOURCE_DIR}/CMakeLists.txt)
        add_subdirectory(${mongoose_SOURCE_DIR} ${mongoose_BINARY_DIR})

    endif()
    # Group under the "external" project folder in IDEs such as Visual Studio.
    set_property(TARGET mongoose PROPERTY FOLDER "external")
    restore_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        # Disable C++98 compatibility support in clang: https://github.com/libcpr/cpr/issues/927
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat -Wno-c++98-compat-pedantic -Wno-nonportable-system-include-path -Wno-exit-time-destructors -Wno-undef -Wno-global-constructors -Wno-switch-enum -Wno-old-style-cast -Wno-covered-switch-default -Wno-undefined-func-template")
    endif()
endif()

add_subdirectory(cpr)
add_subdirectory(include)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND CPR_BUILD_TESTS)
    # Disable linting for tests since they are currently not up to the standard
    clear_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)
    enable_testing()
    add_subdirectory(test)
    restore_variable(DESTINATION CMAKE_CXX_CLANG_TIDY BACKUP CMAKE_CXX_CLANG_TIDY_BKP)
endif()
