cmake_minimum_required(VERSION 3.15)

target_include_directories(cpr PUBLIC  
    $<INSTALL_INTERFACE:include>    
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)

target_sources(cpr PRIVATE
     # Header files (useful in IDEs)
    cpr/api.h
    cpr/auth.h
    cpr/body.h
    cpr/cookies.h
    cpr/cpr.h
    cpr/cprtypes.h
    cpr/curlholder.h
    cpr/curlholder.h
    cpr/digest.h
    cpr/error.h
    cpr/limit_rate.h
    cpr/max_redirects.h
    cpr/multipart.h
    cpr/ntlm.h
    cpr/parameters.h
    cpr/payload.h
    cpr/proxies.h
    cpr/response.h
    cpr/session.h
    cpr/ssl_options.h
    cpr/timeout.h
    cpr/unix_socket.h
    cpr/util.h
    cpr/verbose.h
)

install(DIRECTORY cpr DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
