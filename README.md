# C++ Requests: Curl for People <img align="right" height="40" src="http://i.imgur.com/d9Xtyts.png">

[![gitter](https://badges.gitter.im/cpp-pm/community.svg)](https://gitter.im/whoshuu/cpr) [![Documentation](https://img.shields.io/badge/documentation-master-brightgreen.svg)](https://whoshuu.github.io/cpr/)
![CI](https://github.com/whoshuu/cpr/workflows/CI/badge.svg) [![Coverage Status](https://coveralls.io/repos/whoshuu/cpr/badge.svg?branch=master&service=github)](https://coveralls.io/github/whoshuu/cpr)

## Announcements

The cpr project will have a new maintainer: [Fabian Sauter](https://github.com/com8) and [Tim Stack](https://github.com/tstack). He has graciously agreed to donate his time to keep the project healthy and grow it. For those waiting on their PRs and issues to be resolved, I appreciate your patience and know that you will be in good hands moving forward.

## TLDR

C++ Requests is a simple wrapper around [libcurl](http://curl.haxx.se/libcurl) inspired by the excellent [Python Requests](https://github.com/kennethreitz/requests) project.

Despite its name, libcurl's easy interface is anything but, and making mistakes misusing it is a common source of error and frustration. Using the more expressive language facilities of C++11, this library captures the essence of making network calls into a few concise idioms.

Here's a quick GET request:

```c++
#include <cpr/cpr.h>

int main(int argc, char** argv) {
    cpr::Response r = cpr::Get(cpr::Url{"https://api.github.com/repos/whoshuu/cpr/contributors"},
                      cpr::Authentication{"user", "pass"},
                      cpr::Parameters{{"anon", "true"}, {"key", "value"}});
    r.status_code;                  // 200
    r.header["content-type"];       // application/json; charset=utf-8
    r.text;                         // JSON text string
}
```

And here's [less functional, more complicated code, without cpr](https://gist.github.com/whoshuu/2dc858b8730079602044).

## Documentation

You can find the latest documentation [here](https://whoshuu.github.io/cpr). It's a work in progress, but it should give you a better idea of how to use the library than the [tests](https://github.com/whoshuu/cpr/tree/master/test) currently do.

## Features

C++ Requests currently supports:

* Custom headers
* Url encoded parameters
* Url encoded POST values
* Multipart form POST upload
* File POST upload
* Basic authentication
* Digest authentication
* NTLM authentication
* Connection and request timeout specification
* Timeout for low speed connection
* Asynchronous requests
* :cookie: support!
* Proxy support
* Callback interface
* PUT methods
* DELETE methods
* HEAD methods
* OPTIONS methods
* PATCH methods
* Thread Safe access to [libCurl](https://curl.haxx.se/libcurl/c/threadsafe.html)
* OpenSSL and WinSSL support for HTTPS requests

## Planned

Support for the following will be forthcoming (in rough order of implementation priority):

* [Streamed requests](https://github.com/whoshuu/cpr/issues/25)

and much more!

## Usage

If you already have a project you need to integrate C++ Requests with, the primary way is to use CMake `fetch_content`.
Add the following to your `CMakeLists.txt`.


```cmake
include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/whoshuu/cpr.git GIT_TAG c8d33915dbd88ad6c92b258869b03aba06587ff9) # the commit hash for 1.5.0
FetchContent_MakeAvailable(cpr)
```

This will produce the target `cpr::cpr` which you can link against the typical way:

```cmake
target_link_libraries(your_target_name PRIVATE cpr::cpr)
```

That should do it!
There's no need to handle `libcurl` yourself. All dependencies are taken care of for you.

## Requirements

The only explicit requirements are:

* a `C++11` compatible compiler such as Clang or GCC. The minimum required version of GCC is unknown, so if anyone has trouble building this library with a specific version of GCC, do let me know
* If you would like to perform https requests `OpenSSL` and its development libraries are required.

## Building cpr - Using vcpkg

You can download and install cpr using the [vcpkg](https://github.com/Microsoft/vcpkg) dependency manager:
```Bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg integrate install
./vcpkg install cpr
```
The cpr port in vcpkg is kept up to date by Microsoft team members and community contributors. If the version is out of date, please [create an issue or pull request](https://github.com/Microsoft/vcpkg) on the vcpkg repository.

## Building cpr - Using Conan

You can download and install `cpr` using the [Conan](https://conan.io/) package manager. Setup your CMakeLists.txt (see [Conan documentation](https://docs.conan.io/en/latest/integrations/build_system.html) on how to use MSBuild, Meson and others) like this:

```CMake
project(myproject CXX)

add_executable(${PROJECT_NAME} main.cpp)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake) # Include Conan-generated file
conan_basic_setup(TARGETS) # Introduce Conan-generated targets

target_link_libraries(${PROJECT_NAME} CONAN_PKG::cpr)
```
Create `conanfile.txt` in your source dir:
```
[requires]
cpr/1.5.0

[generators]
cmake
```
Install and run Conan, then build your project as always:

```Bash
pip install conan
mkdir build
cd build
conan install ../ --build=missing
cmake ../
cmake --build .
```
The `cpr` package in Conan is kept up to date by Conan contributors. If the version is out of date, please [create an issue or pull request](https://github.com/conan-io/conan-center-index) on the `conan-center-index` repository.
