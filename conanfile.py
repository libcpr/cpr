from conans import ConanFile, CMake
import os


class CPRConan(ConanFile):
    name = "cpr"
    version = "1.2.0"
    url = "https://github.com/whoshuu/cpr.git"
    license = "MIT"
    requires = "libcurl/7.47.1@lasote/stable"
    settings = "os", "compiler", "build_type", "arch"
    options = {"insecure_curl": [True, False],
               "use_ssl": [True, False],
               "use_system_curl": [True, False]}
    default_options = "libcurl:with_ldap=False", "libcurl:shared=False", "insecure_curl=False", "use_ssl=True", "use_system_curl=False"
    generators = "cmake"
    exports = ["*"]  # Useful while develop, get the code from the current project directory

    def config(self):
        if self.options.use_ssl:
            self.options["libcurl"].with_openssl = True
        else:
            self.options["libcurl"].with_openssl = False

        if self.options.use_system_curl:
            del self.requires["libcurl"]

    def build(self):
        if not os.path.exists("./build"):
            os.mkdir("./build")
        os.chdir("./build")
        cmake = CMake(self.settings)
        insecure_curl = "-DINSECURE_CURL=ON" if self.options.insecure_curl else ""
        self.output.warn("Building in directory: %s" % self.conanfile_directory)
        self.run('cmake -DCONAN=ON -DUSE_SYSTEM_CURL=ON -DBUILD_CPR_TESTS=OFF -DGENERATE_COVERAGE=OFF %s "%s" %s' % (insecure_curl, self.conanfile_directory, cmake.command_line))
        self.run('cmake --build . %s' % cmake.build_config)

    def package(self):
        self.copy("*.h", dst="include", src="include")
        self.copy("*.lib", dst="lib", src="build/lib")
        self.copy("*.a", dst="lib", src="build/lib")

    def package_info(self):
        self.cpp_info.libs = ["cpr"]
        if self.options.use_system_curl:
            self.cpp_info.libs.append("curl")
