cmake_minimum_required(VERSION 3.15)

message(STATUS "Using CURL_INCLUDE_DIRS: ${CURL_INCLUDE_DIRS}.")

add_library(cpr
    auth.cpp
    cookies.cpp
    cprtypes.cpp
    curlholder.cpp
    digest.cpp
    error.cpp
    multipart.cpp
    ntlm.cpp
    parameters.cpp
    payload.cpp
    proxies.cpp
    session.cpp
    timeout.cpp
    unix_socket.cpp
    util.cpp
    )

add_library(cpr::cpr ALIAS cpr)

message(STATUS "Using CURL_LIBRARIES: ${CURL_LIBRARIES}.")
target_link_libraries(cpr PUBLIC CURL::libcurl) # todo should be private, but first dependencys in ssl_options need to be removed


# Set version for shared libraries.
set_target_properties(cpr
     PROPERTIES
     VERSION ${${PROJECT_NAME}_VERSION}
     SOVERSION ${${PROJECT_NAME}_VERSION_MAJOR})


