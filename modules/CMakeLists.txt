cmake_minimum_required(VERSION 3.28)

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
