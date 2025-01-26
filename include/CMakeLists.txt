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
    cpr/async_wrapper.h
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
    cpr/filesystem.h
    cpr/curlmultiholder.h
    cpr/multiperform.h
    cpr/resolve.h
    ${PROJECT_BINARY_DIR}/cpr_generated_includes/cpr/cprver.h
)

# Filesystem
if(CPR_USE_BOOST_FILESYSTEM)
  find_package(Boost 1.77 REQUIRED COMPONENTS filesystem)
  if(Boost_FOUND)
    target_link_libraries(cpr PUBLIC Boost::filesystem)
  endif()
else()
  try_compile(
    STD_FS_SUPPORTED
    "${CMAKE_CURRENT_BINARY_DIR}/std_fs"
    SOURCES "${PROJECT_SOURCE_DIR}/cmake/std_fs_support_test.cpp"
    CXX_STANDARD ${CMAKE_CXX_STANDARD}
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
  )
  if(NOT STD_FS_SUPPORTED)
    try_compile(
      STDCXXFS_SUPPORTED
      "${CMAKE_CURRENT_BINARY_DIR}/stdcxxfs"
      SOURCES "${PROJECT_SOURCE_DIR}/cmake/std_fs_support_test.cpp"
      LINK_LIBRARIES "stdc++fs"
      CXX_STANDARD ${CMAKE_CXX_STANDARD}
      CXX_STANDARD_REQUIRED ON
      CXX_EXTENSIONS OFF
    )
    if(STDCXXFS_SUPPORTED)
      target_link_libraries(cpr PUBLIC stdc++fs)
    else()
      try_compile(
        CXXFS_SUPPORTED
        "${CMAKE_CURRENT_BINARY_DIR}/cxxfs"
        SOURCES "${PROJECT_SOURCE_DIR}/cmake/std_fs_support_test.cpp"
        LINK_LIBRARIES "c++fs"
        CXX_STANDARD ${CMAKE_CXX_STANDARD}
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
      )
      if(CXXFS_SUPPORTED)
        target_link_libraries(cpr PUBLIC c++fs)
      else()
        message(FATAL_ERROR "Your compiler does not support the `<filesystem>` include or cpr is unable to determine the proper compiler flags for it.")
      endif()
    endif()
  endif()
endif()

install(DIRECTORY cpr DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY ${PROJECT_BINARY_DIR}/cpr_generated_includes/cpr DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
