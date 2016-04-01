cmake_minimum_required(VERSION 3.2)
project(cpr_demo)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include(conanbuildinfo.cmake)
conan_basic_setup()

add_executable(cpr_demo main.cpp)
target_link_libraries(cpr_demo ${CONAN_LIBS})

macro(ADD_FRAMEWORK fwname appname)
    find_library(FRAMEWORK_${fwname}
    NAMES ${fwname}
    PATHS ${CMAKE_OSX_SYSROOT}/System/Library
    PATH_SUFFIXES Frameworks
    NO_DEFAULT_PATH)
    if( ${FRAMEWORK_${fwname}} STREQUAL FRAMEWORK_${fwname}-NOTFOUND)
        MESSAGE(ERROR ": Framework ${fwname} not found")
    else()
        TARGET_LINK_LIBRARIES(${appname} "${FRAMEWORK_${fwname}}/${fwname}")
        MESSAGE(STATUS "Framework ${fwname} found at ${FRAMEWORK_${fwname}}")
    endif()
endmacro(ADD_FRAMEWORK)

if(APPLE)
    ADD_FRAMEWORK(Cocoa cpr_demo)
    ADD_FRAMEWORK(Security cpr_demo)
endif()

