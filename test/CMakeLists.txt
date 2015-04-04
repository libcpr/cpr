include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CPR_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
    ${MONGOOSE_INCLUDE_DIRS})

add_executable(get_tests
    get_tests.cpp server.cpp)

target_link_libraries(get_tests
    ${GTEST_LIBRARIES}
    ${CPR_LIBRARIES}
    ${MONGOOSE_LIBRARIES})

add_executable(post_tests
    post_tests.cpp server.cpp)

target_link_libraries(post_tests
    ${GTEST_LIBRARIES}
    ${CPR_LIBRARIES}
    ${MONGOOSE_LIBRARIES})

add_executable(session_tests
    session_tests.cpp server.cpp)

target_link_libraries(session_tests
    ${GTEST_LIBRARIES}
    ${CPR_LIBRARIES}
    ${MONGOOSE_LIBRARIES})

add_test(NAME cpr_get_tests COMMAND get_tests)
add_test(NAME cpr_post_tests COMMAND post_tests)
add_test(NAME cpr_session_tests COMMAND session_tests)
