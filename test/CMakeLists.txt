macro(add_cpr_test _TEST_NAME)
    add_executable(${_TEST_NAME}_tests
        ${_TEST_NAME}_tests.cpp server.cpp server.h)
    target_link_libraries(${_TEST_NAME}_tests
        ${GTEST_LIBRARIES}
        ${CPR_LIBRARIES}
        ${MONGOOSE_LIBRARIES})
    add_test(NAME cpr_${_TEST_NAME}_tests COMMAND ${_TEST_NAME}_tests)
    # Group under the "tests" project folder in IDEs such as Visual Studio.
    set_property(TARGET ${_TEST_NAME}_tests PROPERTY FOLDER "tests")
    if(WIN32)
        add_custom_command(TARGET ${_TEST_NAME}_tests POST_BUILD
                           COMMAND ${CMAKE_COMMAND} -E copy
                                $<TARGET_FILE_DIR:libcurl>/libcurl.dll $<TARGET_FILE_DIR:${_TEST_NAME}_tests>)
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
