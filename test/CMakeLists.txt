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

add_test(NAME get_api_tests COMMAND get_tests)
add_test(NAME post_api_tests COMMAND post_tests)

if(GENERATE_COVERAGE)
    #    setup_target_for_coverage(${PROJECT_NAME}_coverage ctest coverage -V 
    #    "'gtest/*';'googletest/*';'mongoose/*';'curl/*';'test/*'")
endif()
