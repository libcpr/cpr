cmake_minimum_required(VERSION 3.15)
project(cpr VERSION 1.5.1 LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

# Avoid the dll boilerplate code for windows
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(CPR_LIBRARIES cpr CACHE INTERNAL "")

macro(cpr_option OPTION_NAME OPTION_TEXT OPTION_DEFAULT)
    option(${OPTION_NAME} ${OPTION_TEXT} ${OPTION_DEFAULT})
    if(DEFINED ENV{${OPTION_NAME}})
        # Allow setting the option through an environment variable
        set(${OPTION_NAME} $ENV{${OPTION_NAME}})
    endif()
    if(${OPTION_NAME})
        add_definitions(-D${OPTION_NAME})
    endif()
    message(STATUS "  ${OPTION_NAME}: ${${OPTION_NAME}}")
endmacro()

option(${BUILD_SHARED_LIBS} "Build libraries as shared libraries" ON)
message(STATUS "C++ Requests CMake Options")
message(STATUS "=======================================================")
cpr_option(USE_SYSTEM_CURL
    "If ON, this project will look in the system paths for an installed curl library" OFF)
cpr_option(BUILD_CPR_TESTS "Set to ON to build cpr tests." ON)
cpr_option(BUILD_CPR_TESTS_SSL "Set to ON to build cpr ssl tests" ${BUILD_CPR_TESTS})
cpr_option(GENERATE_COVERAGE "Set to ON to generate coverage reports." OFF)
cpr_option(CPR_CURL_NOSIGNAL "Set to ON to disable use of signals in libcurl." OFF)
cpr_option(USE_SYSTEM_GTEST
    "If ON, this project will look in the system paths for an installed gtest library" OFF)
cpr_option(USE_OPENSSL "Use OpenSSL code. Experimental" ON)
cpr_option(USE_WINSSL "Use WIN_SSL backend. Experimental" OFF)
message(STATUS "=======================================================")

if (USE_OPENSSL AND USE_WINSSL)
    message(FATAL_ERROR "It's only possible to use one SSL backend")
endif()

if ((NOT WIN32) AND USE_WINSSL)
    message(FATAL_ERROR "Only use WINSSL with windows")
endif()


include(GNUInstallDirs)
include(FetchContent)
include(cmake/code_coverage.cmake)

# Curl configuration
if(USE_SYSTEM_CURL)
    find_package(CURL COMPONENTS PROTOCOLS HTTP HTTPS FEATURES SSL)
    if (CURL_FOUND)
        set(SSL_ENABLED ON CACHE INTERNAL "" FORCE)
    else()
        find_package(CURL COMPONENTS PROTOCOLS HTTP)
        if(CURL_FOUND)
            set(SSL_ENABLED OFF CACHE INTERNAL "" FORCE)
        endif()
    endif()
endif()
if(NOT USE_SYSTEM_CURL OR NOT CURL_FOUND)
    message(STATUS "Not using system Curl, using built-in curl project instead.")
    
    set(BUILD_CURL_EXE OFF CACHE INTERNAL "" FORCE)
    set(BUILD_TESTING OFF CACHE INTERNAL "" FORCE)
    set(HTTP_ONLY ON CACHE INTERNAL "" FORCE)

    if (USE_WINSSL OR USE_OPENSSL)
        set(SSL_ENABLED ON CACHE INTERNAL "" FORCE)
    else()
        set(CURL_CA_PATH "none" CACHE INTERNAL "" FORCE)
    endif()

    if(USE_WINSSL)
        set(CMAKE_USE_WINSSL ON CACHE INTERNAL "" FORCE)
        set(CURL_CA_PATH "none" CACHE INTERNAL "" FORCE)
    endif()

    if(USE_OPENSSL) 
        set(CMAKE_USE_OPENSSL ON CACHE INTERNAL "" FORCE)
    endif()

    # Show progress of FetchContent:
    set(FETCHCONTENT_QUIET OFF CACHE INTERNAL "" FORCE)
    FetchContent_Declare(curl
                         GIT_REPOSITORY         https://github.com/curl/curl.git
                         GIT_TAG                b81e0b07784dc4c1e8d0a86194b9d28776d071c0 # the hash for curl-7_69_1
                         GIT_PROGRESS           TRUE
                         USES_TERMINAL_DOWNLOAD TRUE)   # <---- This is needed only for Ninja to show download progress

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
                             GIT_REPOSITORY         https://github.com/google/googletest.git
                             GIT_TAG                703bd9caab50b139428cea1aaff9974ebee5742e # the hash for release-1.10.0
                             GIT_PROGRESS           TRUE
                             USES_TERMINAL_DOWNLOAD TRUE)   # <---- This is needed only for Ninja to show download progress
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
    if (SSL_ENABLED)
        find_package(OpenSSL)
        if (OPENSSL_FOUND)
            set(ENABLE_SSL_TESTS ${BUILD_CPR_TESTS_SSL} CACHE INTERNAL "")
        else()
            set(ENABLE_SSL_TESTS OFF CACHE INTERNAL "")
        endif()
    else()
            set(ENABLE_SSL_TESTS OFF CACHE INTERNAL "")
    endif()

    FetchContent_Declare(mongoose 
                         GIT_REPOSITORY         https://github.com/cesanta/mongoose.git
                         GIT_TAG                80d74e9e341d541f71c0fa587d22cec89be32dd5 # the hash for 6.18
                         GIT_PROGRESS           TRUE
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
endif()


add_subdirectory(cpr)
add_subdirectory(include)

if(BUILD_CPR_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
