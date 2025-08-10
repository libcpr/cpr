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

string(TOLOWER "${CMAKE_SYSTEM_NAME}" MESON_TARGET_HOST_SYSTEM_NAME)
string(TOLOWER "${CMAKE_SYSTEM_PROCESSOR}" MESON_TARGET_SYSTEM_PROCESSOR_LOWER)
if(MESON_TARGET_SYSTEM_PROCESSOR_LOWER MATCHES "^(x86_64|amd64)$")
    set(MESON_TARGET_HOST_CPU_FAMILY "x86_64")
elseif(MESON_TARGET_SYSTEM_PROCESSOR_LOWER MATCHES "^(i.86|x86)$")
    set(MESON_TARGET_HOST_CPU_FAMILY "x86")
elseif(MESON_TARGET_SYSTEM_PROCESSOR_LOWER MATCHES "^(armv7|armv6|arm)$")
    set(MESON_TARGET_HOST_CPU_FAMILY "arm")
elseif(MESON_TARGET_SYSTEM_PROCESSOR_LOWER MATCHES "^(aarch64|arm64)$")
    set(MESON_TARGET_HOST_CPU_FAMILY "aarch64")
else()
    set(MESON_TARGET_HOST_CPU_FAMILY "${MESON_TARGET_SYSTEM_PROCESSOR_LOWER}")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(MESON_BUILD_TYPE debug)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(MESON_BUILD_TYPE debugoptimized)
else()
     set(MESON_BUILD_TYPE release)
endif()

include (TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
    set(MESON_ENDIAN "big")
else()
    set(MESON_ENDIAN "little")
endif()

# libpsl is plain C. Make sure CMake initializes a C tool-chain.
if(NOT CMAKE_C_COMPILER)
    enable_language(C) # initializes CMAKE_C_COMPILER, CMAKE_AR, …
endif()

# Write a meson cross compilation file to allow cross compiling
# for example for building NuGet packages although usually it is not required.
file(WRITE "${CMAKE_BINARY_DIR}/libpsl-meson-cross.txt" "[binaries]
c = '${CMAKE_C_COMPILER}'
cpp = '${CMAKE_CXX_COMPILER}'
ar = '${CMAKE_AR}'
strip = '${CMAKE_STRIP}'

[host_machine]
system = '${MESON_TARGET_HOST_SYSTEM_NAME}'
cpu_family = '${MESON_TARGET_HOST_CPU_FAMILY}'
cpu = '${MESON_TARGET_HOST_CPU_FAMILY}'
endian = '${MESON_ENDIAN}'
")

# Meson configure
# We only care about static libraries of psl. In case you need a dynamic version, feel free to add support for it.
message(STATUS "Configuring libpsl...")
execute_process(COMMAND "${MESON_PATH}" setup
                        "${LIBPSL_BUILD_DIR}"
                        "${LIBPSL_SOURCE_DIR}"
                        -Dtests=false
                        -Ddocs=false
                        --cross-file "${CMAKE_BINARY_DIR}/libpsl-meson-cross.txt"
                        --buildtype=${MESON_BUILD_TYPE}
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

list(APPEND CMAKE_INCLUDE_PATH "${LIBPSL_INSTALL_DIR}/include")

if(EXISTS "${LIBPSL_INSTALL_DIR}/lib64")
    set(LIBPSL_LIBRARY "${LIBPSL_INSTALL_DIR}/lib/libpsl.a")
    list(APPEND CMAKE_LIBRARY_PATH "${LIBPSL_INSTALL_DIR}/lib64")
else()
    set(LIBPSL_LIBRARY "${LIBPSL_INSTALL_DIR}/lib/libpsl.a")
    list(APPEND CMAKE_LIBRARY_PATH "${LIBPSL_INSTALL_DIR}/lib")
endif()

set(LIBPSL_INCLUDE_DIR  "${LIBPSL_INSTALL_DIR}/include")

# Workaround for Windows compilation.
# Ref: https://github.com/microsoft/vcpkg/pull/38847/files#diff-922fe829582a7e5acf5b0c35181daa63064fc12a2c889c5d89a19e5e02113f1bL44
add_compile_definitions(PSL_STATIC=1)
