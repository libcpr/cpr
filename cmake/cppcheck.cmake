# Include this file if and only if you want to use cppcheck.

if (NOT ${CPR_CPPCHECK_PATH} STREQUAL "")
    get_filename_component(CPPCHECK_HINT_FILENAME ${CPR_CPPCHECK_PATH} NAME)
    get_filename_component(CPPCHECK_HINT_PATH ${CPR_CPPCHECK_PATH} DIRECTORY)
endif ()

find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck ${CPPCHECK_HINT_FILENAME} HINTS ${CPPCHECK_HINT_PATH} REQUIRED)
message(STATUS "Found cppcheck: ${CMAKE_CXX_CPPCHECK}")

list(APPEND CMAKE_CXX_CPPCHECK
        "--xml"
        "--error-exitcode=1"
        "--enable=warning,style"
        "--force"
        "--inline-suppr"
        "--addon=y2038"
        "--std=c++${CMAKE_CXX_STANDARD}"
        "--cppcheck-build-dir=${PROJECT_BINARY_DIR}"
        "--suppress-xml=${PROJECT_SOURCE_DIR}/cppcheck-suppressions.xml"
        "--output-file=${PROJECT_BINARY_DIR}/cppcheck.xml"
        "--check-level=normal"
)
