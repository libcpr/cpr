find_package(Threads)
set(TEST_SERVER_LIBRARY test_server)
add_library(${TEST_SERVER_LIBRARY}
    server.cpp)
target_link_libraries(${TEST_SERVER_LIBRARY}
    ${MONGOOSE_LIBRARIES}
    ${CMAKE_THREAD_LIBS_INIT})

macro(add_cpr_test _TEST_NAME)
    add_executable(${_TEST_NAME}_tests
        ${_TEST_NAME}_tests.cpp)
    target_link_libraries(${_TEST_NAME}_tests
        ${TEST_SERVER_LIBRARY}
        ${GTEST_LIBRARIES}
        ${CPR_LIBRARIES})
    add_test(NAME cpr_${_TEST_NAME}_tests COMMAND ${_TEST_NAME}_tests)
    # Group under the "tests" project folder in IDEs such as Visual Studio.
    set_property(TARGET ${_TEST_NAME}_tests PROPERTY FOLDER "tests")
    if(WIN32)
        set(LIBCURL_DLL_NAME "libcurl.dll")
        if(CMAKE_BUILD_TYPE STREQUAL "DEBUG")
            set(LIBCURL_DLL_NAME "libcurl-d.dll")
        endif()
        add_custom_command(TARGET ${_TEST_NAME}_tests POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy
                                $<TARGET_FILE_DIR:libcurl>/${LIBCURL_DLL_NAME} $<TARGET_FILE_DIR:${_TEST_NAME}_tests>)
    endif()
endmacro()

include_directories(
    ${CURL_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CPR_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
    ${MONGOOSE_INCLUDE_DIRS})

add_cpr_test(get)
add_cpr_test(post)
add_cpr_test(session)
add_cpr_test(async)
add_cpr_test(proxy)
add_cpr_test(head)
add_cpr_test(delete)
add_cpr_test(put)
add_cpr_test(callback)
add_cpr_test(raw_body)
add_cpr_test(options)
add_cpr_test(patch)
add_cpr_test(error)
add_cpr_test(alternating)
add_cpr_test(util)
