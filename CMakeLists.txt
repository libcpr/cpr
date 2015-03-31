cmake_minimum_required(VERSION 3.2)
project(cpr)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

find_package(CURL)
if(NOT CURL_FOUND)
    message(STATUS "Could not find system Curl, using built-in curl project instead.")
    add_subdirectory(curl)
    set(CURL_LIBRARIES libcurl CACHE INTERNAL "")
    set(CURL_INCLUDE_DIRS
        ${CURL_BINARY_DIR}/include/curl
        ${CURL_SOURCE_DIR}/include CACHE INTERNAL "")
endif()

set(CPR_LIBRARIES libcpr CACHE INTERNAL "")
set(CPR_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "")

add_subdirectory(cpr)
