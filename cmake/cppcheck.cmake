find_program(CMAKE_CXX_CPPCHECK NAMES cppcheck)
if(CMAKE_CXX_CPPCHECK)
    list(APPEND CMAKE_CXX_CPPCHECK 
        "--error-exitcode=1"
        "--enable=warning,style"
        "--force" 
        "--inline-suppr"
        "--std=c++${CMAKE_CXX_STANDARD}"
        "--suppressions-list=${CMAKE_SOURCE_DIR}/CppCheckSuppressions.txt")
endif()
