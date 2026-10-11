# Locate wolfSSL using the same variables as curl's FindWolfSSL module.
find_path(WOLFSSL_INCLUDE_DIR NAMES wolfssl/ssl.h)
find_library(WOLFSSL_LIBRARY NAMES wolfssl)

if(WOLFSSL_INCLUDE_DIR AND EXISTS "${WOLFSSL_INCLUDE_DIR}/wolfssl/version.h")
    file(STRINGS "${WOLFSSL_INCLUDE_DIR}/wolfssl/version.h" _wolfssl_version_line
        REGEX "^#define LIBWOLFSSL_VERSION_STRING")
    string(REGEX REPLACE ".*\"([^\"]+)\".*" "\\1" WOLFSSL_VERSION "${_wolfssl_version_line}")
    unset(_wolfssl_version_line)
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(WolfSSL
    REQUIRED_VARS WOLFSSL_INCLUDE_DIR WOLFSSL_LIBRARY
    VERSION_VAR WOLFSSL_VERSION)

if(WolfSSL_FOUND)
    set(WOLFSSL_INCLUDE_DIRS "${WOLFSSL_INCLUDE_DIR}")
    set(WOLFSSL_LIBRARIES "${WOLFSSL_LIBRARY}")
    if(NOT WIN32)
        find_library(WOLFSSL_MATH_LIBRARY NAMES m)
        if(WOLFSSL_MATH_LIBRARY)
            list(APPEND WOLFSSL_LIBRARIES "${WOLFSSL_MATH_LIBRARY}")
        endif()
    endif()
endif()

mark_as_advanced(WOLFSSL_INCLUDE_DIR WOLFSSL_LIBRARY WOLFSSL_MATH_LIBRARY)
