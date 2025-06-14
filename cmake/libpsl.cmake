# Builds libpsl which is especially necessary on Windows since there it is not available via e.g. a package manager.

include(ExternalProject)
find_program(MESON_PATH meson)

if(MESON_PATH STREQUAL "MESON_PATH-NOTFOUND")
    message(FATAL_ERROR "meson not found. Please make sure you have meson installed on your system (https://mesonbuild.com/Getting-meson.html). Meson is required for building libpsl for curl on Windows.")
    return()
endif()

FetchContent_Declare(libpsl_src GIT_REPOSITORY https://github.com/rockdaboot/libpsl.git
                                GIT_TAG 0.21.5)
FetchContent_MakeAvailable(libpsl_src) # sets libpsl_src_SOURCE_DIR / _BINARY_DIR

set(LIBPSL_SOURCE_DIR "${libpsl_src_SOURCE_DIR}")
set(LIBPSL_BUILD_DIR "${libpsl_src_BINARY_DIR}")
set(LIBPSL_INSTALL_DIR "${CMAKE_BINARY_DIR}/libpsl_src-install")
file(MAKE_DIRECTORY "${LIBPSL_BUILD_DIR}")

# Meson configure
# We only care about static libraries of psl. In case you need a dynamic version, feel free to add support for it.
message(STATUS "Configuring libpsl...")
execute_process(COMMAND "${MESON_PATH}" setup
                        "${LIBPSL_BUILD_DIR}"
                        "${LIBPSL_SOURCE_DIR}"
                        -Dtests=false
                        -Ddocs=false
                        --buildtype=release
                        --prefix "${LIBPSL_INSTALL_DIR}"
                        --default-library=static
                RESULT_VARIABLE MESON_SETUP_RC)
if(MESON_SETUP_RC)
    message(FATAL_ERROR "Meson setup for libpsl failed!")
endif()

# Meson build
message(STATUS "Building libpsl...")
execute_process(COMMAND "${MESON_PATH}" compile -C "${LIBPSL_BUILD_DIR}"
                RESULT_VARIABLE MESON_COMPILE_RC
)
if(MESON_COMPILE_RC)
    message(FATAL_ERROR "Meson compile for libpsl failed!")
endif()

# Meson install
message(STATUS "Installing libpsl...")
execute_process(COMMAND "${MESON_PATH}" install -C "${LIBPSL_BUILD_DIR}"
                RESULT_VARIABLE MESON_INSTALL_RC)
if(MESON_INSTALL_RC)
    message(FATAL_ERROR "Meson install for libpsl failed!")
endif()

list(APPEND CMAKE_LIBRARY_PATH "${LIBPSL_INSTALL_DIR}/lib64")
list(APPEND CMAKE_LIBRARY_PATH "${LIBPSL_INSTALL_DIR}/lib")
list(APPEND CMAKE_INCLUDE_PATH "${LIBPSL_INSTALL_DIR}/include")

# Workaround for Windows compilation.
# Ref: https://github.com/microsoft/vcpkg/pull/38847/files#diff-922fe829582a7e5acf5b0c35181daa63064fc12a2c889c5d89a19e5e02113f1bL44
add_compile_definitions(PSL_STATIC=1)
