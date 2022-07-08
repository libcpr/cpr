cmake_minimum_required(VERSION 3.15)

add_library(cpr
        accept_encoding.cpp
        async.cpp
        auth.cpp
        bearer.cpp
        cert_info.cpp
        cookies.cpp
        cprtypes.cpp
        curl_container.cpp
        curlholder.cpp
        error.cpp
        multipart.cpp
        parameters.cpp
        payload.cpp
        proxies.cpp
        proxyauth.cpp
        session.cpp
        threadpool.cpp
        timeout.cpp
        unix_socket.cpp
        util.cpp
        response.cpp
        redirect.cpp
        interceptor.cpp
        ssl_ctx.cpp)

add_library(cpr::cpr ALIAS cpr)

target_link_libraries(cpr PUBLIC CURL::libcurl) # todo should be private, but first dependencys in ssl_options need to be removed

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

if(CPR_FORCE_USE_SYSTEM_CURL)
        install(TARGETS cpr
                EXPORT cprTargets
                RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
                ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

        # Include CMake helpers for package config files
        # Follow this installation guideline: https://cmake.org/cmake/help/latest/manual/cmake-packages.7.html
        include(CMakePackageConfigHelpers)

        write_basic_package_version_file(
                "${PROJECT_BINARY_DIR}/cpr/cprConfigVersion.cmake"
                VERSION ${${PROJECT_NAME}_VERSION}
                COMPATIBILITY ExactVersion)

        configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/cprConfig.cmake.in
                "${PROJECT_BINARY_DIR}/cpr/cprConfig.cmake"
                INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpr)

        install(EXPORT cprTargets
                FILE cprTargets.cmake
                NAMESPACE cpr::
                DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpr)

        install(FILES ${PROJECT_BINARY_DIR}/cpr/cprConfig.cmake
                ${PROJECT_BINARY_DIR}/cpr/cprConfigVersion.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cpr)

else()
        install(TARGETS cpr
                RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
                ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()
