from conans import ConanFile, CMake

class CPRConan(ConanFile):
    name = "cpr"
    version = "1.2.0"
    url = "https://github.com/whoshuu/cpr.git"
    license = "MIT"
    requires = "libcurl/7.47.1@lasote/stable"
    settings = "os", "compiler", "build_type", "arch"
    options = {"build_cpr_tests": [True, False],
               "insecure_curl": [True, False],
               "generate_coverage": [True, False],
               "use_openssl": [True, False]}
    default_options = "build_cpr_tests=False", "insecure_curl=False", "generate_coverage=False", "use_openssl=True"

    def source(self):
        self.run("git clone https://github.com/DEGoodmanWilson/cpr.git --branch conan")

    def config(self):
        if self.options.use_openssl:
            self.options["libcurl"].with_openssl = True
        else:
            self.options["libcurl"].with_openssl = False

        if self.options.build_cpr_tests:
            self.requires.add("gtest/1.7.0@lasote/stable", private=False)
            #self.options["gtest"].shared = self.options.shared
            self.options["gtest"].shared = False
        else:
            del self.requires["gtest"]

        #TODO if building test, add gtest!

    def build(self):
        cmake = CMake(self.settings)
        build_tests = "-DBUILD_CPR_TESTS=OFF" if not self.options.build_cpr_tests else ""
        insecure_curl = "-DINSECURE_CURL=ON" if self.options.insecure_curl else ""
        generate_coverage = "-DGENERATE_COVERAGE=ON" if self.options.generate_coverage else ""

        self.run('cmake -DUSE_SYSTEM_CURL=ON %s %s %s "%s/cpr" %s' % (build_tests, insecure_curl, generate_coverage, self.conanfile_directory, cmake.command_line))
        self.run('cmake --build . %s' % cmake.build_config)

    def package(self):
        self.copy("*.h", dst="include/cpr", src="cpr/include")
        self.copy("*.lib", dst="lib", src="lib")
        self.copy("*.a", dst="lib", src="lib")

    def package_info(self):
        self.cpp_info.libs = ["cpr"]