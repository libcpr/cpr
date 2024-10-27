# C++ Requests: Curl for People <img align="right" height="40" src="http://i.imgur.com/d9Xtyts.png">

[![Documentation](https://img.shields.io/badge/docs-online-informational?style=flat&link=https://docs.libcpr.org/)](https://docs.libcpr.org/)
![CI](https://github.com/libcpr/cpr/workflows/CI/badge.svg)
[![Gitter](https://badges.gitter.im/libcpr/community.svg)](https://gitter.im/libcpr/community?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge)

## Announcements

* This project is being maintained by [Fabian Sauter](https://github.com/com8) and [Kilian Traub](https://github.com/KingKili).
* For quick help, and discussion libcpr also offers a [gitter](https://gitter.im/libcpr/community?utm_source=share-link&utm_medium=link&utm_campaign=share-link) chat.

## Supported Releases
| Release  | Min. C++ Standard | Status | Notes |
|----------|-------------------|--------|-------|
| master   | `cpp17` | ![alt text][preview] | |
| 1.11.x   | `cpp17` | ![alt text][supported] | |
| 1.10.x   | `cpp17` | ![alt text][unsupported] | |
| 1.9.x    | `cpp11` | ![alt text][supported] | Supported until 01.01.2025 |
| <= 1.8.x | `cpp11` | ![alt text][unsupported] | |

[unsupported]: https://img.shields.io/badge/-unsupported-red "unsupported"
[supported]: https://img.shields.io/badge/-supported-green "supported"
[preview]: https://img.shields.io/badge/-preview-orange "preview"

## TLDR

C++ Requests is a simple wrapper around [libcurl](http://curl.haxx.se/libcurl) inspired by the excellent [Python Requests](https://github.com/kennethreitz/requests) project.

Despite its name, libcurl's easy interface is anything but, and making mistakes, misusing it is a common source of error and frustration. Using the more expressive language facilities of `C++17` (or `C++11` in case you use cpr < 1.10.0), this library captures the essence of making network calls into a few concise idioms.

Here's a quick GET request:

```c++
#include <cpr/cpr.h>

int main(int argc, char** argv) {
    cpr::Response r = cpr::Get(cpr::Url{"https://api.github.com/repos/whoshuu/cpr/contributors"},
                      cpr::Authentication{"user", "pass", cpr::AuthMode::BASIC},
                      cpr::Parameters{{"anon", "true"}, {"key", "value"}});
    r.status_code;                  // 200
    r.header["content-type"];       // application/json; charset=utf-8
    r.text;                         // JSON text string
    return 0;
}
```

And here's [less functional, more complicated code, without cpr](https://gist.github.com/whoshuu/2dc858b8730079602044).

## Documentation

[![Documentation](https://img.shields.io/badge/docs-online-informational?style=for-the-badge&link=https://docs.libcpr.org/)](https://docs.libcpr.org/)  
You can find the latest documentation [here](https://docs.libcpr.org/). It's a work in progress, but it should give you a better idea of how to use the library than the [tests](https://github.com/libcpr/cpr/tree/master/test) currently do.

## Features

C++ Requests currently supports:

* Custom headers
* URL-encoded parameters
* URL-encoded POST values
* Multipart form POST upload
* File POST upload
* Basic authentication
* Bearer authentication
* Digest authentication
* NTLM authentication
* Connection and request timeout specification
* Timeout for low speed connection
* Asynchronous requests
* :cookie: support!
* Proxy support
* Callback interfaces
* PUT methods
* DELETE methods
* HEAD methods
* OPTIONS methods
* PATCH methods
* Thread Safe access to [libCurl](https://curl.haxx.se/libcurl/c/threadsafe.html)
* OpenSSL and WinSSL support for HTTPS requests

## Planned

For a quick overview about the planned features, have a look at the next [Milestones](https://github.com/libcpr/cpr/milestones).

## Usage

### CMake

#### fetch_content:
If you already have a CMake project you need to integrate C++ Requests with, the primary way is to use `fetch_content`.
Add the following to your `CMakeLists.txt`.


```cmake
include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
                         GIT_TAG bb01c8db702fb41e5497aee9c0559ddf4bf13749) # Replace with your desired git commit from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)
```

This will produce the target `cpr::cpr` which you can link against the typical way:

```cmake
target_link_libraries(your_target_name PRIVATE cpr::cpr)
```

That should do it!
There's no need to handle `libcurl` yourself. All dependencies are taken care of for you.
All of this can be found in an example [**here**](https://github.com/libcpr/example-cmake-fetch-content).

#### find_package():
If you prefer not to use `fetch_content`, you can download, build, and install the library and then use CMake `find_package()` function to integrate it into a project.

**Note:** this feature is feasible only if CPR_USE_SYSTEM_CURL is set. (see [#645](https://github.com/libcpr/cpr/pull/645))
```Bash
git clone https://github.com/libcpr/cpr.git
cd cpr && mkdir build && cd build
cmake .. -DCPR_USE_SYSTEM_CURL=ON
cmake --build . --parallel
sudo cmake --install .
```
#### Build Static Library
As an alternative if you want to switch between a static or shared version of cpr use ['-DBUILD_SHARED_LIBS=ON/OFF'](https://cmake.org/cmake/help/latest/variable/BUILD_SHARED_LIBS.html).
```Bash
git clone https://github.com/libcpr/cpr.git
cd cpr && mkdir build && cd build
cmake .. -DCPR_USE_SYSTEM_CURL=ON -DBUILD_SHARED_LIBS=OFF
cmake --build . --parallel
sudo cmake --install .
```

In your `CMakeLists.txt`:
```cmake
find_package(cpr REQUIRED)
add_executable(your_target_name your_target_name.cpp)
target_link_libraries(your_target_name PRIVATE cpr::cpr)
```

#### Tests
`cpr` provides a bunch of tests that can be executed via the following commands.
```Bash
git clone https://github.com/libcpr/cpr.git
cd cpr && mkdir build && cd build
cmake .. -DCPR_BUILD_TESTS=ON # There are other test related options like 'CPR_BUILD_TESTS_SSL' and 'CPR_BUILD_TESTS_PROXY'
cmake --build . --parallel
ctest -VV # -VV is optional since it enables verbose output
```

### Bazel

Please refer to [hedronvision/bazel-make-cc-https-easy](https://github.com/hedronvision/bazel-make-cc-https-easy).

### Packages for Linux Distributions

Alternatively, you may install a package specific to your Linux distribution. Since so few distributions currently have a package for cpr, most users will not be able to run your program with this approach.

Currently, we are aware of packages for the following distributions:

* [Arch Linux (AUR)](https://aur.archlinux.org/packages/cpr)
* [Fedora Linux](https://src.fedoraproject.org/rpms/cpr)

If there's no package for your distribution, try making one! If you do, and it is added to your distribution's repositories, please submit a pull request to add it to the list above. However, please only do this if you plan to actively maintain the package.

### NuGet Package

For Windows, there is also a libcpr NuGet package available. Currently, x86 and x64 builds are supported with release and debug configuration.

The package can be found here: [NuGet.org](https://www.nuget.org/packages/libcpr/)

### Port for macOS

On macOS you may install cpr via [MacPorts.org](https://ports.macports.org/port/cpr) (arm64, x86_64, powerpc)

### FreeBSD Port

On FreeBSD, you can issue `pkg install cpr` or use the Ports tree to install it.

## Requirements

The only explicit requirements are:

* a `C++17` compatible compiler such as Clang or GCC. The minimum required version of GCC is unknown, so if anyone has trouble building this library with a specific version of GCC, do let us know
* in case you only have a `C++11` compatible compiler available, all versions below cpr 1.9.x are for you. The 1.10.0 release of cpr switches to `C++17` as a requirement.
* If you would like to perform https requests `OpenSSL` and its development libraries are required.
* If you do not use the built-in version of [curl](https://github.com/curl/curl) but instead use your systems version, make sure you use a version `>= 7.64.0`. Lower versions are not supported. This means you need Debian `>= 10` or Ubuntu `>= 20.04 LTS`.

## Building cpr - Using vcpkg

You can download and install cpr using the [vcpkg](https://github.com/Microsoft/vcpkg) dependency manager:
```Bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
./vcpkg install cpr
```
The `cpr` port in vcpkg is kept up to date by Microsoft team members and community contributors. If the version is out of date, please [create an issue or pull request](https://github.com/Microsoft/vcpkg) on the vcpkg repository.

## Building cpr - Using Conan

You can download and install `cpr` using the [Conan](https://conan.io/) package manager. Setup your CMakeLists.txt (see [Conan documentation](https://docs.conan.io/en/latest/integrations/build_system.html) on how to use MSBuild, Meson and others).
An example can be found [**here**](https://github.com/libcpr/example-cmake-conan).

The `cpr` package in Conan is kept up to date by Conan contributors. If the version is out of date, please [create an issue or pull request](https://github.com/conan-io/conan-center-index) on the `conan-center-index` repository.
