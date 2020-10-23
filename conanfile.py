import re
import os
import shutil
from conans import ConanFile, CMake, tools
from conans.errors import ConanInvalidConfiguration

class InfluxdbCxxConan(ConanFile):
    name = "influxdb-cxx"
    license = "MIT"
    author = "offa <offa@github>"
    url = "https://github.com/offa/influxdb-cxx"
    hompage = url
    description = "InfluxDB C++ client library."
    topics = ("influxdb", "influxdb-client")
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    options = {"shared": [True, False],
               "tests": [True, False],
               "boost": [True, False]}
    default_options = {"shared": False,
                       "tests": False,
                       "boost": True,
                       "boost:shared": True,
                       "libcurl:with_openssl": True,
                       "libcurl:shared": True}
    exports = ["LICENSE"]
    exports_sources = ("CMakeLists.txt", "src/*", "include/*", "test/*", "cmake/*", "conan/CMakeLists.txt")
    requires = (
        "libcurl/7.72.0"
    )


    def set_version(self):
        cmake_lists_content = tools.load(os.path.join(self.recipe_folder, "CMakeLists.txt"))
        project_match = re.search(r'project\s*\((.+?)\)', cmake_lists_content, re.DOTALL)

        if not project_match:
            raise ConanInvalidConfiguration("No valid project() statement found in CMakeLists.txt")

        project_params = project_match.group(1).split()
        version_string = project_params[project_params.index("VERSION") + 1]

        if not re.search(r'\d+\.\d+\.\d+(?:\.\d)?', version_string):
            raise ConanInvalidConfiguration("No valid version found in CMakeLists.txt")

        self.version = version_string
        self.output.info("Project version from CMakeLists.txt: '{}'".format(self.version))

    def requirements(self):
        if self.options.boost:
            self.requires("boost/1.74.0")

    def source(self):
        # Wrap the original CMake file to call conan_basic_setup
        shutil.move("CMakeLists.txt", "CMakeListsOriginal.txt")
        shutil.move(os.path.join("conan", "CMakeLists.txt"), "CMakeLists.txt")

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()

        if self.options.tests:
            cmake.test()

    def package(self):
        cmake = self._configure_cmake()
        cmake.install()
        self.copy("LICENSE", dst="licenses")

    def package_info(self):
        self.cpp_info.libs = tools.collect_libs(self)

    def _configure_cmake(self):
        cmake = CMake(self)
        cmake.definitions["INFLUXCXX_TESTING"] = self.options.tests
        cmake.configure(build_folder="build")
        return cmake
