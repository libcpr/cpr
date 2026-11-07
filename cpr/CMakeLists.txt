cmake_minimum_required(VERSION 3.15)

add_library(cpr
        accept_encoding.cpp
        async.cpp
        auth.cpp
        callback.cpp
        cert_info.cpp
        connection_pool.cpp
        cookies.cpp
        cprtypes.cpp
        curl_container.cpp
        curlholder.cpp
        error.cpp
        file.cpp
        multipart.cpp
        parameters.cpp
        payload.cpp
        proxies.cpp
        proxyauth.cpp
        session.cpp
        sse.cpp
        threadpool.cpp
        timeout.cpp
        unix_socket.cpp
        util.cpp
        response.cpp
        redirect.cpp
        interceptor.cpp
        ssl_ctx.cpp
        curlmultiholder.cpp
        multiperform.cpp)

add_library(cpr::cpr ALIAS cpr)

# Consumers of DLL data need explicit import declarations even with automatic exports.
# More: https://cmake.org/cmake/help/latest/module/GenerateExportHeader.html
include(GenerateExportHeader)
generate_export_header(cpr EXPORT_FILE_NAME "${PROJECT_BINARY_DIR}/cpr_generated_includes/cpr/cprexport.h")

find_package(Threads REQUIRED)
target_link_libraries(cpr PRIVATE Threads::Threads)
if(CPR_THREAD_POOL STREQUAL "BOOST_ASIO")
        # Asio and Boost.System are header-only at this version; do not fetch Boost.
        find_package(Boost 1.70 REQUIRED)
        target_sources(cpr PRIVATE threadpool_boost.cpp threadpool_boost_context.cpp)
        target_link_libraries(cpr PRIVATE Boost::headers)
else()
        target_sources(cpr PRIVATE threadpool_builtin.cpp)
endif()

target_link_libraries(cpr PUBLIC ${CURL_LIB}) # todo should be private, but first dependencies in ssl_options need to be removed

# Fix missing OpenSSL includes for Windows since in 'ssl_ctx.cpp' we include OpenSSL directly
if(SSL_BACKEND_USED STREQUAL "OpenSSL")
        target_link_libraries(cpr PRIVATE OpenSSL::SSL)
        target_include_directories(cpr PRIVATE ${OPENSSL_INCLUDE_DIR})
endif()

# Set version for shared libraries.
set_target_properties(cpr
        PROPERTIES
        VERSION ${${PROJECT_NAME}_VERSION}
        SOVERSION ${${PROJECT_NAME}_VERSION_MAJOR})

# Import GNU common install directory variables
include(GNUInstallDirs)

if (CPR_ENABLE_INSTALL)
        install(TARGETS cpr
                EXPORT cprTargets
                RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
                ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

if(CPR_USE_SYSTEM_CURL)
        # Include CMake helpers for package config files
        # Follow this installation guideline: https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html
        include(CMakePackageConfigHelpers)

        write_basic_package_version_file(
                "${PROJECT_BINARY_DIR}/cpr/cprConfigVersion.cmake"
                VERSION ${${PROJECT_NAME}_VERSION}
                COMPATIBILITY ExactVersion)

        if(SSL_BACKEND_USED STREQUAL "OpenSSL")
                configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/cprConfig-ssl.cmake.in
                        "${PROJECT_BINARY_DIR}/cpr/cprConfig.cmake"
                        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpr)
        else()
                configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/cprConfig.cmake.in
                        "${PROJECT_BINARY_DIR}/cpr/cprConfig.cmake"
                        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpr)
        endif()

        if (CPR_ENABLE_INSTALL)
                install(FILES ${PROJECT_BINARY_DIR}/cpr/cprConfig.cmake
                        ${PROJECT_BINARY_DIR}/cpr/cprConfigVersion.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpr)
        endif()
endif()

if (CPR_ENABLE_INSTALL)
        install(EXPORT cprTargets
                FILE cprTargets.cmake
                NAMESPACE cpr::
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpr)
endif()
