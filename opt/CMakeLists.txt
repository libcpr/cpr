# Curl configuration

if(USE_SYSTEM_CURL)
    find_package(CURL)
endif()
if(NOT USE_SYSTEM_CURL OR NOT CURL_FOUND)
    message(STATUS "Not using system Curl, using built-in curl project instead.")
    add_subdirectory(curl)
    set(CURL_FOUND TRUE)
    set(CURL_LIBRARIES libcurl)
    set(CURL_INCLUDE_DIRS
        ${CURL_BINARY_DIR}/include
        ${CURL_SOURCE_DIR}/include/curl)
endif()

set(CURL_FOUND ${CURL_FOUND} CACHE INTERNAL "Set if libcurl is found or built")
set(CURL_LIBRARIES ${CURL_LIBRARIES} CACHE INTERNAL "Location of libcurl")
set(CURL_INCLUDE_DIRS ${CURL_INCLUDE_DIRS} CACHE INTERNAL "Location of curl include files")


# GTest configuration

if(BUILD_CPR_TESTS)
    if(USE_SYSTEM_GTEST)
        find_package(GTest)
    endif()
    if(NOT USE_SYSTEM_GTEST OR NOT GTEST_FOUND)
        message(STATUS "Not using system gtest, using built-in googletest project instead.")
        add_subdirectory(googletest)
        set(GTEST_FOUND TRUE)
        set(GTEST_LIBRARIES gtest)
        set(GTEST_MAIN_LIBRARIES gtest_main)
        set(GTEST_BOTH_LIBRARIES gtest gtest_main)
        set(GTEST_INCLUDE_DIRS ${gtest_SOURCE_DIR}/include)
    endif()

    set(GTEST_FOUND ${GTEST_FOUND} CACHE INTERNAL "Set if libgtest was found or built")
    set(GTEST_LIBRARIES ${GTEST_LIBRARIES} CACHE INTERNAL "Location of libgtest")
    set(GTEST_MAIN_LIBRARIES ${GTEST_MAIN_LIBRARIES} CACHE INTERNAL "Location of libgtest-main")
    set(GTEST_BOTH_LIBRARIES ${GTEST_BOTH_LIBRARIES} CACHE INTERNAL "Location of both gtest libraries")
    set(GTEST_INCLUDE_DIRS ${GTEST_INCLUDE_DIRS} CACHE INTERNAL "Location of gtest include files")
endif()
