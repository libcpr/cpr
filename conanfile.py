from conans import ConanFile, CMake

class CPRConan(ConanFile):
    name = "cpr"
    version = "1.2.0"
    url = "https://github.com/whoshuu/cpr.git"
    license = "MIT"
    requires = "libcurl/7.47.1@lasote/stable"
    settings = "os", "compiler", "build_type", "arch"
    options = {"insecure_curl": [True, False],
               "use_ssl": [True, False]}
    default_options = "libcurl:shared=False", "insecure_curl=False", "use_ssl=True"
    generators = "cmake"
    exports = ["./*"] # Useful while develop, get the code from the curren project directory
    src_folder = "" if exports else "cpr/"


    def config(self):
        if self.options.use_ssl:
            self.options["libcurl"].with_openssl = True
        else:
            self.options["libcurl"].with_openssl = False

    def build(self):
        cmake = CMake(self.settings)
        insecure_curl = "-DINSECURE_CURL=ON" if self.options.insecure_curl else ""

        self.run('cmake -DUSE_SYSTEM_CURL=ON -DBUILD_CPR_TESTS=OFF -DGENERATE_COVERAGE=OFF %s "%s/cpr" %s' % (insecure_curl, self.conanfile_directory, cmake.command_line))
        self.run('cmake --build . %s' % cmake.build_config)

    def package(self):
        self.copy("*.h", dst="include", src="cpr/include")
        self.copy("*.lib", dst="lib", src="lib")
        self.copy("*.a", dst="lib", src="lib")

    def package_info(self):
        self.cpp_info.libs = ["cpr"]
