# Include this file if and only if you want to use clang-tidy linter.

if (NOT ${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    if (NOT ${CPR_LINTER_PATH} STREQUAL "")
        get_filename_component(CLANG_TIDY_HINT_FILENAME ${CPR_LINTER_PATH} NAME)
        get_filename_component(CLANG_TIDY_HINT_PATH ${CPR_LINTER_PATH} DIRECTORY)
    endif ()

    find_program(CLANG_TIDY_EXECUTABLE NAMES clang-tidy ${CLANG_TIDY_HINT_FILENAME} HINTS ${CLANG_TIDY_HINT_PATH} REQUIRED)
    mark_as_advanced(CLANG_TIDY_EXECUTABLE)

    message(STATUS "Enabling clang-tidy: ${CLANG_TIDY_EXECUTABLE}")
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXECUTABLE};-warnings-as-errors=*")

    # If enabled, clang-tidy will try to apply suggested fixes
    if (CPR_ENABLE_LINTING_AUTO_FIX)
        set(CMAKE_CXX_CLANG_TIDY "${CMAKE_CXX_CLANG_TIDY};--fix;--fix-errors;--fix-notes")
    endif ()
else ()
    message(FATAL_ERROR "Clang-tidy is not supported when building for windows")
endif ()
