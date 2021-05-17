cmake_minimum_required(VERSION 3.15)

find_package(Threads REQUIRED)

if (ENABLE_SSL_TESTS)
    add_library(test_server STATIC
                abstractServer.cpp
                httpServer.cpp
                httpsServer.cpp)
else ()
    add_library(test_server STATIC
                abstractServer.cpp
                httpServer.cpp)
endif()
if(WIN32)
    target_link_libraries(test_server PRIVATE Threads::Threads cpr::cpr GTest::GTest
                                      PUBLIC mongoose ws2_32 wsock32)
else()
    target_link_libraries(test_server PRIVATE Threads::Threads cpr::cpr GTest::GTest
                                      PUBLIC mongoose)
endif()

macro(add_cpr_test _TEST_NAME)
    add_executable(${_TEST_NAME}_tests ${_TEST_NAME}_tests.cpp)
    target_link_libraries(${_TEST_NAME}_tests PRIVATE
        test_server
        GTest::GTest
        cpr::cpr
        CURL::libcurl)
    add_test(NAME cpr_${_TEST_NAME}_tests COMMAND ${_TEST_NAME}_tests)
    # Group under the "tests" project folder in IDEs such as Visual Studio.
    set_property(TARGET ${_TEST_NAME}_tests PROPERTY FOLDER "tests")
    if(WIN32 AND BUILD_SHARED_LIBS)
        add_custom_command(TARGET ${_TEST_NAME}_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:libcurl> $<TARGET_FILE_DIR:${_TEST_NAME}_tests>)
        add_custom_command(TARGET ${_TEST_NAME}_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:cpr> $<TARGET_FILE_DIR:${_TEST_NAME}_tests>)
    endif()
endmacro()

add_cpr_test(get)
add_cpr_test(post)
add_cpr_test(session)
add_cpr_test(prepare)
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
add_cpr_test(structures)
add_cpr_test(encoded_auth)
add_cpr_test(proxy_auth)

if (ENABLE_SSL_TESTS)
    add_cpr_test(ssl)
    # Install all ssl keys and certs:

    add_custom_command(TARGET ssl_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:ssl_tests>/data)
    add_custom_command(TARGET ssl_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/data/server.pubkey $<TARGET_FILE_DIR:ssl_tests>)
    add_custom_command(TARGET ssl_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/data/server.key $<TARGET_FILE_DIR:ssl_tests>)
    add_custom_command(TARGET ssl_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/data/server.cer $<TARGET_FILE_DIR:ssl_tests>)
    add_custom_command(TARGET ssl_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/data/client.key $<TARGET_FILE_DIR:ssl_tests>)
    add_custom_command(TARGET ssl_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/data/client.cer $<TARGET_FILE_DIR:ssl_tests>)
endif()
