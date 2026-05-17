module;

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <curl/curl.h>
#include <curl/curlver.h>
#include <curl/system.h>
#include <sys/types.h>
#ifdef CPR_USE_BOOST_FILESYSTEM
#define BOOST_FILESYSTEM_VERSION 4
#include <boost/filesystem.hpp>
#endif

export module cpr;

import std;

#define CPR_AS_MODULE 1
#define CPR_IMPORT_STD 1
#define EXPORT_CPR export

extern "C++" {
#include "cpr/cpr.h"
}
