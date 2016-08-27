message(STATUS "Using CURL_INCLUDE_DIRS: ${CURL_INCLUDE_DIRS}.")
include_directories(
    ${CPR_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS})

add_library(${CPR_LIBRARIES}

    # Source files
    auth.cpp
    cookies.cpp
    cprtypes.cpp
    digest.cpp
    error.cpp
    multipart.cpp
    parameters.cpp
    payload.cpp
    proxies.cpp
    session.cpp
    util.cpp

    # Header files (useful in IDEs)
    "${CPR_INCLUDE_DIRS}/cpr/api.h"
    "${CPR_INCLUDE_DIRS}/cpr/auth.h"
    "${CPR_INCLUDE_DIRS}/cpr/body.h"
    "${CPR_INCLUDE_DIRS}/cpr/cookies.h"
    "${CPR_INCLUDE_DIRS}/cpr/cpr.h"
    "${CPR_INCLUDE_DIRS}/cpr/cprtypes.h"
    "${CPR_INCLUDE_DIRS}/cpr/curlholder.h"
    "${CPR_INCLUDE_DIRS}/cpr/defines.h"
    "${CPR_INCLUDE_DIRS}/cpr/digest.h"
    "${CPR_INCLUDE_DIRS}/cpr/error.h"
    "${CPR_INCLUDE_DIRS}/cpr/max_redirects.h"
    "${CPR_INCLUDE_DIRS}/cpr/multipart.h"
    "${CPR_INCLUDE_DIRS}/cpr/parameters.h"
    "${CPR_INCLUDE_DIRS}/cpr/payload.h"
    "${CPR_INCLUDE_DIRS}/cpr/proxies.h"
    "${CPR_INCLUDE_DIRS}/cpr/response.h"
    "${CPR_INCLUDE_DIRS}/cpr/session.h"
    "${CPR_INCLUDE_DIRS}/cpr/timeout.h"
    "${CPR_INCLUDE_DIRS}/cpr/util.h")

message(STATUS "Using CURL_LIBRARIES: ${CURL_LIBRARIES}.")
target_link_libraries(${CPR_LIBRARIES}
    ${CURL_LIBRARIES})

# Layout. This works for all platforms:
#   * <prefix>/lib/cmake/<PROJECT-NAME>
#   * <prefix>/lib/
#   * <prefix>/include/
set(config_install_dir "lib/cmake/${PROJECT_NAME}")
set(include_install_dir "include")

set(generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated")

# Configuration
set(version_config "${generated_dir}/${PROJECT_NAME}ConfigVersion.cmake")
set(project_config "${generated_dir}/${PROJECT_NAME}Config.cmake")
set(targets_export_name "${PROJECT_NAME}Targets")
set(namespace "${PROJECT_NAME}::")

# Include module with fuction 'write_basic_package_version_file'
include(CMakePackageConfigHelpers)

# Configure '<PROJECT-NAME>ConfigVersion.cmake'
# Note: PROJECT_VERSION is used as a VERSION
write_basic_package_version_file(
    "${version_config}" COMPATIBILITY SameMajorVersion
)

# Configure '<PROJECT-NAME>Config.cmake'
# Use variables:
#   * targets_export_name
#   * PROJECT_NAME
configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/../cmake/cpr-config.cmake.in"
    "${project_config}"
    INSTALL_DESTINATION "${config_install_dir}"
)


# Targets:
install(
    TARGETS ${CPR_LIBRARIES} 
    EXPORT "${targets_export_name}"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
    RUNTIME DESTINATION "bin"
    INCLUDES DESTINATION "${include_install_dir}"
)

# Headers:
install(
    DIRECTORY "${CPR_INCLUDE_DIRS}/cpr"
    DESTINATION "${include_install_dir}"
    FILES_MATCHING PATTERN "*.h"
)

# Config
#   * <prefix>/lib/cmake/Foo/FooConfig.cmake
#   * <prefix>/lib/cmake/Foo/FooConfigVersion.cmake
#   * <prefix>/lib/cmake/Foo/FooTargets.cmake
install(
    FILES "${project_config}" "${version_config}"
    DESTINATION "${config_install_dir}"
)
install(
    EXPORT "${targets_export_name}"
    NAMESPACE "${namespace}"
    DESTINATION "${config_install_dir}"
)
