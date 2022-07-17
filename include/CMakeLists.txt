cmake_minimum_required(VERSION 3.15)

target_include_directories(cpr PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/cpr_generated_includes/>)

target_sources(cpr PRIVATE

    # Header files (useful in IDEs)
    cpr/accept_encoding.h
    cpr/api.h
    cpr/async.h
    cpr/auth.h
    cpr/bearer.h
    cpr/body.h
    cpr/buffer.h
    cpr/cert_info.h
    cpr/cookies.h
    cpr/cpr.h
    cpr/cprtypes.h
    cpr/curlholder.h
    cpr/curlholder.h
    cpr/error.h
    cpr/file.h
    cpr/limit_rate.h
    cpr/local_port.h
    cpr/local_port_range.h
    cpr/multipart.h
    cpr/parameters.h
    cpr/payload.h
    cpr/proxies.h
    cpr/proxyauth.h
    cpr/response.h
    cpr/session.h
    cpr/singleton.h
    cpr/ssl_ctx.h
    cpr/ssl_options.h
    cpr/threadpool.h
    cpr/timeout.h
    cpr/unix_socket.h
    cpr/util.h
    cpr/verbose.h
    cpr/interface.h
    cpr/redirect.h
    cpr/http_version.h
    cpr/interceptor.h
    ${PROJECT_BINARY_DIR}/cpr_generated_includes/cpr/cprver.h
)

install(DIRECTORY cpr DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY ${PROJECT_BINARY_DIR}/cpr_generated_includes/cpr DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
