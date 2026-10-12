function(expect_configuration name expected_success)
    execute_process(COMMAND "${CMAKE_COMMAND}" -E env
        --unset=CPR_USE_EXISTING_CURL_TARGET --unset=CPR_ENABLE_SSL
        --unset=CPR_BUILD_TESTS --unset=CPR_BUILD_TESTS_SSL --unset=CPR_ENABLE_INSTALL
        --unset=CPR_USE_BOOST_FILESYSTEM
        "${CMAKE_COMMAND}" -S "${CPR_SOURCE_DIR}/test/cmake/threadpool_configure"
        -B "${CPR_TEST_BINARY_DIR}/fixture-${name}" "-DCPR_SOURCE_DIR=${CPR_SOURCE_DIR}" ${ARGN}
        RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
    if(expected_success AND NOT result EQUAL 0)
        message(FATAL_ERROR "${name} unexpectedly failed: ${output}\n${error}")
    elseif(NOT expected_success AND result EQUAL 0)
        message(FATAL_ERROR "${name} unexpectedly succeeded")
    endif()
    set(configuration_output "${output}\n${error}" PARENT_SCOPE)
endfunction()

# Leaving the selector unset must work without Boost installed or discoverable.
expect_configuration(default TRUE -DCMAKE_DISABLE_FIND_PACKAGE_Boost=TRUE)
expect_configuration(invalid FALSE -DCPR_THREAD_POOL=invalid)
if(NOT configuration_output MATCHES "Invalid CPR_THREAD_POOL")
    message(FATAL_ERROR "Invalid backend did not produce the expected diagnostic")
endif()
expect_configuration(missing_boost FALSE -DCPR_THREAD_POOL=BOOST_ASIO
    -DCMAKE_DISABLE_FIND_PACKAGE_Boost=TRUE)
if(NOT configuration_output MATCHES "Boost")
    message(FATAL_ERROR "Missing Boost did not produce a dependency diagnostic")
endif()
