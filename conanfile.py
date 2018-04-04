from conans import ConanFile, tools, CMake
import os

class cprConan(ConanFile):
    name = "cpr"
    version = "1.3.0"
    license = "MIT"
    url = "https://github.com/whoshuu/cpr"
    description = "C++ Requests: Curl for People, a spiritual port of Python Requests"
    settings = "os", "compiler", "build_type", "arch"
    exports_sources = "*"
    requires = "libcurl/7.56.1@bincrafters/stable", "OpenSSL/1.0.2@conan/stable"
    generators = "cmake"
    options = {
        "shared"            : [True, False],
        "enable_ssl"        : [True, False],
        "enable_system_curl"        : [True, False]
    }
    default_options = (
        "shared=False",
        "enable_ssl=True",
        "enable_system_curl=True"
    )

    def optionBool(self, b):
        if b:
            return "ON"
        else:
            return "OFF"

    def parseOptionsToCMake(self):
        cmakeOpts = {
            "BUILD_CPR_TESTS" : "OFF"
        }

        cmakeOpts["BUILD_SHARED_LIBS"] = self.optionBool(self.options.shared)
        cmakeOpts["USE_SYSTEM_CURL"] = self.optionBool(self.options.enable_system_curl)

        return cmakeOpts

    def config(self):
        pass

    def build(self):
        cmake = CMake(self)
        os.makedirs("./buildit")
        cmake.configure(defs=self.parseOptionsToCMake(), build_dir="./buildit")
        cmake.build()
        cmake.install()

    def package(self):
        # nothing to do here now
        pass

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)
        self.cpp_info.includedirs = ["include"]
