message(STATUS "Using CURL_INCLUDE_DIRS: ${CURL_INCLUDE_DIRS}.")
include_directories(
    ${CPR_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS})

add_library(${CPR_LIBRARIES}

    # Source files
    auth.cpp
    cookies.cpp
    cprtypes.cpp
    digest.cpp
    multipart.cpp
    parameters.cpp
    payload.cpp
    proxies.cpp
    session.cpp
    util.cpp 

    # Header files (useful in IDEs)
    "${CPR_INCLUDE_DIRS}/api.h"
    "${CPR_INCLUDE_DIRS}/auth.h"
    "${CPR_INCLUDE_DIRS}/body.h"
    "${CPR_INCLUDE_DIRS}/cookies.h"
    "${CPR_INCLUDE_DIRS}/cpr.h"
    "${CPR_INCLUDE_DIRS}/cprtypes.h"
    "${CPR_INCLUDE_DIRS}/curlholder.h"
    "${CPR_INCLUDE_DIRS}/defines.h"
    "${CPR_INCLUDE_DIRS}/digest.h"
    "${CPR_INCLUDE_DIRS}/multipart.h"
    "${CPR_INCLUDE_DIRS}/parameters.h"
    "${CPR_INCLUDE_DIRS}/payload.h"
    "${CPR_INCLUDE_DIRS}/proxies.h"
    "${CPR_INCLUDE_DIRS}/response.h"
    "${CPR_INCLUDE_DIRS}/session.h"
    "${CPR_INCLUDE_DIRS}/timeout.h"
    "${CPR_INCLUDE_DIRS}/util.h")

message(STATUS "Using CURL_LIBRARIES: ${CURL_LIBRARIES}.")
target_link_libraries(${CPR_LIBRARIES}
    ${CURL_LIBRARIES})
