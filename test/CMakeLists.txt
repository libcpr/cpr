include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CPR_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
    ${GTEST_INCLUDE_DIRS}
    ${MONGOOSE_INCLUDE_DIRS})

add_executable(get_tests
    get_tests.cpp server.cpp)

target_link_libraries(get_tests
    ${GTEST_LIBRARIES}
    ${CPR_LIBRARIES}
    ${MONGOOSE_LIBRARIES})

add_test(NAME cpr_tests COMMAND get_tests)

if(GENERATE_COVERAGE)
    setup_target_for_coverage(${PROJECT_NAME}_coverage get_tests coverage ""
        "'gtest/*';'googletest/*';'mongoose/*';'curl/*'")
endif()
