message(STATUS "Using CURL_INCLUDE_DIRS: ${CURL_INCLUDE_DIRS}.")
include_directories(
    ${CPR_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS})

set(CPR_SRC_FILES    
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
    util.cpp)

set(CPR_HEADER_FILES    
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

add_library(${CPR_LIBRARIES} ${CPR_SRC_FILES} ${CPR_HEADER_FILES})

message(STATUS "Using CURL_LIBRARIES: ${CURL_LIBRARIES}.")
target_link_libraries(${CPR_LIBRARIES} ${CURL_LIBRARIES})

set_target_properties(${CPR_LIBRARIES} PROPERTIES
                      PUBLIC_HEADER "${CPR_HEADER_FILES}"
                      DEBUG_POSTFIX d
                      LINKER_LANGUAGE CXX)

# Installation.
install(TARGETS ${CPR_LIBRARIES}
        EXPORT cpr-targets
        RUNTIME DESTINATION ${INSTALL_BIN_DIR} COMPONENT bin
        LIBRARY DESTINATION ${INSTALL_LIB_DIR} COMPONENT shlib
        ARCHIVE DESTINATION ${INSTALL_LIB_DIR} COMPONENT lib
        PUBLIC_HEADER DESTINATION ${INSTALL_INCLUDE_DIR}/cpr
        COMPONENT dev)
