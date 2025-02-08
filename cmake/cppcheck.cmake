find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)

if(CMAKE_CXX_CPPCHECK)
    list(APPEND CMAKE_CXX_CPPCHECK "--xml"
        "--error-exitcode=1"
        "--enable=warning,style"
        "--force"
        "--inline-suppr"
        "--addon=y2038"
        "--std=c++${CMAKE_CXX_STANDARD}"
        "--cppcheck-build-dir=${PROJECT_BINARY_DIR}"
        "--suppress-xml=${PROJECT_SOURCE_DIR}/cppcheck-suppressions.xml"
        "--output-file=${PROJECT_BINARY_DIR}/cppcheck.xml"
        "--check-level=normal")
endif()
