# C++ Requests: Curl for People <img align="right" height="40" src="http://i.imgur.com/d9Xtyts.png">

[![gitter](https://badges.gitter.im/cpp-pm/community.svg)](https://gitter.im/whoshuu/cpr) [![Documentation](https://img.shields.io/badge/documentation-master-brightgreen.svg)](https://whoshuu.github.io/cpr/)
[![Build Status](https://travis-ci.org/whoshuu/cpr.svg?branch=master)](https://travis-ci.org/whoshuu/cpr) [![Build status](https://ci.appveyor.com/api/projects/status/imalkp3a6hblpj5y/branch/master?svg=true)](https://ci.appveyor.com/project/whoshuu/cpr/branch/master) [![Coverage Status](https://coveralls.io/repos/whoshuu/cpr/badge.svg?branch=master&service=github)](https://coveralls.io/github/whoshuu/cpr)

## Announcements

The cpr project will have a new maintainer: [Tim Stack](https://github.com/tstack). He has graciously agreed to donate his time to keep the project healthy and grow it. For those waiting on their PRs and issues to be resolved, I appreciate your patience and know that you will be in good hands moving forward.

## TLDR

C++ Requests is a simple wrapper around [libcurl](http://curl.haxx.se/libcurl) inspired by the excellent [Python Requests](https://github.com/kennethreitz/requests) project.

Despite its name, libcurl's easy interface is anything but, and making mistakes misusing it is a common source of error and frustration. Using the more expressive language facilities of C++11, this library captures the essence of making network calls into a few concise idioms.

Here's a quick GET request:

```c++
#include <cpr/cpr.h>

int main(int argc, char** argv) {
    auto r = cpr::Get(cpr::Url{"https://api.github.com/repos/whoshuu/cpr/contributors"},
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

## Planned

Support for the following will be forthcoming (in rough order of implementation priority):

* [Streamed requests](https://github.com/whoshuu/cpr/issues/25)
* [OpenSSL support](https://github.com/whoshuu/cpr/issues/31)

and much more!

## Usage

For just getting this library up and running, I highly recommend forking the [example project](https://github.com/whoshuu/cpr-example). It's configured with the minimum CMake magic and boilerplate needed to start playing around with networked applications.

If you already have a project you need to integrate C++ Requests with, the primary way is to use cmake fetch_content. Just add this code to you CMakeLists.txt.


```cmake
    include(FetchContent)
    FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/whoshuu/cpr.git)
    FetchContent_MakeAvailable(cpr)

```

This will produce the target `cpr::cpr` which you can link against the typical way:

```cmake
target_link_libraries(your_target_name PRIVATE cpr::cpr)
```

and that should do it! Using the submodule method of integrating C++ Requests, there's no need to handle libcurl yourself, all of those dependencies are taken care of for you.

## Requirements

The only explicit requirements are:

* a C++11 compatible compiler such as Clang or GCC. The minimum required version of GCC is unknown, so if anyone has trouble building this library with a specific version of GCC, do let me know
* curl and its development libraries

## Building cpr - Using vcpkg

You can download and install cpr using the [vcpkg](https://github.com/Microsoft/vcpkg) dependency manager:

    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    ./bootstrap-vcpkg.sh
    ./vcpkg integrate install
    ./vcpkg install cpr

The cpr port in vcpkg is kept up to date by Microsoft team members and community contributors. If the version is out of date, please [create an issue or pull request](https://github.com/Microsoft/vcpkg) on the vcpkg repository.
