set(CPACK_PACKAGE_NAME "libcpr")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "C++ Requests: Curl for People")
set(CPACK_PACKAGE_DESCRIPTION "C++ Requests is a simple wrapper around [libcurl](http://curl.haxx.se/libcurl) inspired by the excellent [Python Requests](https://github.com/kennethreitz/requests) project.")
set(CPACK_PACKAGE_VENDOR "libcpr")
set(CPACK_PACKAGE_CONTACT "Adam Aposhian <aposhian.dev@gmail.com>")
set(CPACK_PACKAGE_DESCRIPTION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/README.md)
set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_PACKAGE_VERSION_MAJOR ${cpr_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${cpr_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${cpr_VERSION_PATCH})
set(CPACK_SOURCE_PACKAGE_FILE_NAME "libcpr-${cpr_VERSION}")

set(CPACK_SOURCE_IGNORE_FILES
  "/\\\\.git"
  "/\\\\.github"
  "/debian/"
  "/.*build-.*/"
  ${PROJECT_BINARY_DIR}
  )

set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

include(CPack)
