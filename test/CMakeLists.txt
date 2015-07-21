macro(add_cpr_test _TEST_NAME)
    add_executable(${_TEST_NAME}_tests
        ${_TEST_NAME}_tests.cpp server.cpp)
    target_link_libraries(${_TEST_NAME}_tests
        ${GTEST_LIBRARIES}
        ${CPR_LIBRARIES}
        ${MONGOOSE_LIBRARIES})
    add_test(NAME cpr_${_TEST_NAME}_tests COMMAND ${_TEST_NAME}_tests)
endmacro()

include_directories(
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
