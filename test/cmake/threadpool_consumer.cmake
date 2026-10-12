function(run)
    execute_process(COMMAND ${ARGV} RESULT_VARIABLE result
        OUTPUT_VARIABLE output ERROR_VARIABLE error)
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "Command failed: ${ARGV}\n${output}\n${error}")
    endif()
endfunction()

set(prefix "${CPR_CONSUMER_BINARY_DIR}/install")
set(config_args)
set(ctest_config_args)
if(CPR_TEST_CONFIG)
    list(APPEND config_args --config "${CPR_TEST_CONFIG}")
    list(APPEND ctest_config_args -C "${CPR_TEST_CONFIG}")
endif()
run("${CMAKE_COMMAND}" --install "${CPR_BUILD_DIR}/cpr" --prefix "${prefix}"
    ${config_args})
run("${CMAKE_COMMAND}" --install "${CPR_BUILD_DIR}/include" --prefix "${prefix}"
    ${config_args})
run("${CMAKE_COMMAND}" -S "${CPR_CONSUMER_SOURCE_DIR}"
    -B "${CPR_CONSUMER_BINARY_DIR}/build" "-DCMAKE_PREFIX_PATH=${prefix}"
    "-DCPR_TEST_CUSTOM_CONTEXT=${CPR_TEST_CUSTOM_CONTEXT}"
    "-DBOOST_ROOT=${CPR_TEST_BOOST_ROOT}"
    "-DBoost_ROOT=${CPR_TEST_BOOST_ROOT}"
    "-DCMAKE_DISABLE_FIND_PACKAGE_Boost=${CPR_TEST_DISABLE_BOOST}")
run("${CMAKE_COMMAND}" --build "${CPR_CONSUMER_BINARY_DIR}/build"
    ${config_args})
run("${CMAKE_CTEST_COMMAND}" --test-dir "${CPR_CONSUMER_BINARY_DIR}/build"
    ${ctest_config_args} --output-on-failure)
