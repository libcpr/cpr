message(STATUS "Using CURL_INCLUDE_DIRS: ${CURL_INCLUDE_DIRS}.")
include_directories(
    ${CPR_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS})

add_library(${CPR_LIBRARIES}
    api.cpp
    auth.cpp
    cprtypes.cpp
    digest.cpp
    multipart.cpp
    parameters.cpp
    payload.cpp
    session.cpp
    util.cpp)

message(STATUS "Using CURL_LIBRARIES: ${CURL_LIBRARIES}.")
target_link_libraries(${CPR_LIBRARIES}
    ${CURL_LIBRARIES})
