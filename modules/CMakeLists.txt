cmake_minimum_required(VERSION 3.28)

# Apple Clang does not ship clang-scan-deps, which CMake requires for module
# dependency scanning.  Use LLVM clang (e.g. from Homebrew) instead.
if(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    message(FATAL_ERROR
        "Apple Clang does not support C++20 module dependency scanning.\n"
        "Use LLVM clang (e.g. 'brew install llvm') and pass "
        "-DCMAKE_CXX_COMPILER=$(brew --prefix llvm)/bin/clang++ to CMake.")
endif()

# GCC gained module scanning support in version 14.
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND
        CMAKE_CXX_COMPILER_VERSION VERSION_LESS "14")
    message(FATAL_ERROR
        "C++20 module scanning requires GCC 14 or later "
        "(found ${CMAKE_CXX_COMPILER_VERSION}).")
endif()

set(CMAKE_CXX_SCAN_FOR_MODULES ON)

add_library(cpr_module)

target_sources(cpr_module
    PUBLIC
        FILE_SET CXX_MODULES FILES
            cpr.cxx
)

target_compile_features(cpr_module PUBLIC cxx_std_20)

target_include_directories(cpr_module PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_library(cpr::module ALIAS cpr_module)

# Installation
install(TARGETS cpr_module
    EXPORT ${PROJECT_NAME}Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    FILE_SET CXX_MODULES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/module
)
