cmake_minimum_required(VERSION 3.15)

find_package(Threads REQUIRED)

if (ENABLE_SSL_TESTS)
    add_library(test_server STATIC
                abstractServer.cpp
                httpServer.cpp
                httpsServer.cpp
                testUtils.cpp)
else ()
    add_library(test_server STATIC
                abstractServer.cpp
                httpServer.cpp
                testUtils.cpp)
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
        ${CURL_LIB})
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
if(CPR_BUILD_TESTS_PROXY)
    add_cpr_test(proxy)
    add_cpr_test(proxy_auth)
endif()
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
add_cpr_test(version)
add_cpr_test(download)
add_cpr_test(interceptor)
add_cpr_test(interceptor_multi)
add_cpr_test(multiperform)
add_cpr_test(resolve)
add_cpr_test(multiasync)
add_cpr_test(file_upload)
if(MSVC)
    # Decode the Unicode fixture names as UTF-8 regardless of the system code page.
    target_compile_options(file_upload_tests PRIVATE /utf-8)
endif()
add_cpr_test(singleton)
add_cpr_test(threadpool)
set_tests_properties(cpr_threadpool_tests PROPERTIES TIMEOUT 60)
if(CPR_THREAD_POOL STREQUAL "BOOST_ASIO")
    find_package(Boost 1.70 REQUIRED)
    target_compile_definitions(threadpool_tests PRIVATE CPR_TEST_BOOST_ASIO)
    target_link_libraries(threadpool_tests PRIVATE Boost::headers)
    add_cpr_test(asio_context)
    target_link_libraries(asio_context_tests PRIVATE Boost::headers)
    add_cpr_test(asio_context_http)
    target_link_libraries(asio_context_http_tests PRIVATE Boost::headers)
    set_tests_properties(cpr_asio_context_tests cpr_asio_context_http_tests PROPERTIES TIMEOUT 60)
endif()
add_test(NAME cpr_threadpool_configuration
    COMMAND ${CMAKE_COMMAND}
        -DCPR_SOURCE_DIR=${PROJECT_SOURCE_DIR}
        -DCPR_TEST_BINARY_DIR=${PROJECT_BINARY_DIR}/threadpool-config-test
        -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/threadpool_configuration.cmake)
if(CPR_ENABLE_INSTALL AND CPR_USE_SYSTEM_CURL)
    add_test(NAME cpr_installed_threadpool_consumer
        COMMAND ${CMAKE_COMMAND}
            -DCPR_BUILD_DIR=${PROJECT_BINARY_DIR}
            -DCPR_CONSUMER_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/cmake/threadpool_consumer
            -DCPR_CONSUMER_BINARY_DIR=${PROJECT_BINARY_DIR}/threadpool-consumer-test
            -DCPR_TEST_CONFIG=$<CONFIG>
            -DCPR_TEST_CUSTOM_CONTEXT=$<STREQUAL:${CPR_THREAD_POOL},BOOST_ASIO>
            -DCPR_TEST_BOOST_ROOT=${Boost_INCLUDE_DIR}
            -DCPR_TEST_DISABLE_BOOST=$<AND:$<STREQUAL:${CPR_THREAD_POOL},BUILTIN>,$<NOT:$<BOOL:${CPR_USE_BOOST_FILESYSTEM}>>>
            -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/threadpool_consumer.cmake)
endif()
add_cpr_test(testUtils)
add_cpr_test(connection_pool)
add_cpr_test(sse)
add_cpr_test(curlholder)

if (ENABLE_SSL_TESTS)
    add_cpr_test(ssl)

    # Install all ssl keys and certs. Explicit copy for each file to prevent issues when copying on Windows.
    add_custom_command(TARGET ssl_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_FILE_DIR:ssl_tests>/data/certificates $<TARGET_FILE_DIR:ssl_tests>/data/keys)
    add_custom_command(TARGET ssl_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/data/certificates/client.crt $<TARGET_FILE_DIR:ssl_tests>/data/certificates/client.crt)
    add_custom_command(TARGET ssl_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/data/certificates/root-ca.crt $<TARGET_FILE_DIR:ssl_tests>/data/certificates/root-ca.crt)
    add_custom_command(TARGET ssl_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/data/certificates/sub-ca.crt $<TARGET_FILE_DIR:ssl_tests>/data/certificates/sub-ca.crt)
    add_custom_command(TARGET ssl_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/data/certificates/ca-bundle.crt $<TARGET_FILE_DIR:ssl_tests>/data/certificates/ca-bundle.crt)
    add_custom_command(TARGET ssl_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/data/certificates/server.crt $<TARGET_FILE_DIR:ssl_tests>/data/certificates/server.crt)
    add_custom_command(TARGET ssl_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/data/keys/client.key $<TARGET_FILE_DIR:ssl_tests>/data/keys/client.key)
    add_custom_command(TARGET ssl_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/data/keys/root-ca.key $<TARGET_FILE_DIR:ssl_tests>/data/keys/root-ca.key)
    add_custom_command(TARGET ssl_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/data/keys/server.key $<TARGET_FILE_DIR:ssl_tests>/data/keys/server.key)
    add_custom_command(TARGET ssl_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/data/keys/server.pub $<TARGET_FILE_DIR:ssl_tests>/data/keys/server.pub)
endif()

add_custom_command(TARGET file_upload_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/data/test_file_hello_äüöp_2585_你好.txt $<TARGET_FILE_DIR:file_upload_tests>/data/test_file_hello_äüöp_2585_你好.txt)
add_custom_command(TARGET file_upload_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/data/test_file_hello_äüöp_2585.txt $<TARGET_FILE_DIR:file_upload_tests>/data/test_file_hello_äüöp_2585.txt)
add_custom_command(TARGET file_upload_tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/data/test_file.txt $<TARGET_FILE_DIR:file_upload_tests>/data/test_file.txt)

file(INSTALL data DESTINATION data)
