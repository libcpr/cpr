cmake_minimum_required(VERSION 3.24)
project(cpr_threadpool_configuration LANGUAGES CXX)

# Configure without downloads or a system curl installation. No compilation is
# needed to verify backend selection and dependency discovery.
add_library(CURL::libcurl INTERFACE IMPORTED GLOBAL)
set(CPR_USE_EXISTING_CURL_TARGET ON CACHE BOOL "" FORCE)
set(CPR_ENABLE_SSL OFF CACHE BOOL "" FORCE)
set(CPR_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(CPR_BUILD_TESTS_SSL OFF CACHE BOOL "" FORCE)
set(CPR_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(CPR_USE_BOOST_FILESYSTEM OFF CACHE BOOL "" FORCE)
add_subdirectory("${CPR_SOURCE_DIR}" cpr)
