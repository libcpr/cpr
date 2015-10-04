# C++ Requests: Curl for People <img align="right" height="40" src="http://i.imgur.com/d9Xtyts.png">

[![Join the chat at https://gitter.im/whoshuu/cpr](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/whoshuu/cpr?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

[![Build Status](https://travis-ci.org/whoshuu/cpr.svg?branch=master)](https://travis-ci.org/whoshuu/cpr) [![Build status](https://ci.appveyor.com/api/projects/status/imalkp3a6hblpj5y/branch/master?svg=true)](https://ci.appveyor.com/project/whoshuu/cpr/branch/master) [![Coverage Status](https://coveralls.io/repos/whoshuu/cpr/badge.svg?branch=master)](https://coveralls.io/r/whoshuu/cpr) [![Documentation](https://img.shields.io/badge/documentation-master-brightgreen.svg)](https://whoshuu.github.io/cpr/)

C++ Requests is a simple wrapper around [libcurl](http://curl.haxx.se/libcurl) inspired by the excellent [Python Requests](https://github.com/kennethreitz/requests) project.

Despite its name, libcurl's easy interface is anything but, and making mistakes misusing it is a common source of error and frustration. Using the more expressive language facilities of C++11, this library captures the essence of making network calls into a few concise idioms.

Here's a quick GET request:

```c++
#include <cpr.h>

int main(int argc, char** argv) {
    auto r = cpr::Get(Url{"https://api.github.com/repos/whoshuu/cpr/contributors"},
                      Authentication{"user", "pass"},
                      Parameters{{"anon", "true"}, {"key", "value"}});
    r.status_code;                  // 200
    r.headers["content-type"];      // application/json; charset=utf-8
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
* Timeout specification
* Asynchronous requests
* :cookie: support!
* Proxy support
* Callback interface
* PUT methods
* DELETE methods
* HEAD methods

## Planned

Support for the following will be forthcoming (in rough order of implementation priority):

* [Streamed requests](https://github.com/whoshuu/cpr/issues/25)
* [OPTIONS methods](https://github.com/whoshuu/cpr/issues/34)
* [PATCH methods](https://github.com/whoshuu/cpr/issues/36)
* [OpenSSL support](https://github.com/whoshuu/cpr/issues/31)

and much more!

## Usage

For just getting this library up and running, I highly recommend forking the [example project](https://github.com/whoshuu/cpr-example). It's configured with the minimum CMake magic and boilerplate needed to start playing around with networked applications.

If you already have a project you need to integrate C++ Requests with, the primary way is to use git submodules. Add this repository as a submodule of your root repository:

```
git submodule add git@github.com:whoshuu/cpr.git
git submodule update --init --recursive
```

Next, add this subdirectory to your CMakeLists.txt before declaring any targets that might use it:

```
add_subdirectory(cpr)
```

This will produce two important CMake variables, `CPR_INCLUDE_DIRS` and `CPR_LIBRARIES`, which you'll use in the typical way:

```
include_directories(${CPR_INCLUDE_DIRS})
target_link_library(your_target_name ${CPR_LIBRARIES})
```

and that should do it! Using the submodule method of integrating C++ Requests, there's no need to handle libcurl yourself, all of those dependencies are taken care of for you.

## Requirements

The only explicit requirement is a C++11 compatible compiler such as clang or gcc. The minimum required version of gcc is unknown, so if anyone has trouble building this library with a specific version of gcc, do let me know.

## Disclaimer

This library is very much in a pre-alpha stage. Please don't attempt to use this in any serious or critical production environment. If you do use it and find bugs you'd like to report, see below!

## Contributing

Please fork this repository and contribute back using [pull requests](https://github.com/whoshuu/cpr/pulls). Features can be requested using [issues](https://github.com/whoshuu/cpr/issues). All code, comments, and critiques are greatly appreciated.
