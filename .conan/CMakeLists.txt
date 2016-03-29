cmake_minimum_required(VERSION 3.2)
project(cpr_demo)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include(conanbuildinfo.cmake)
conan_basic_setup()

add_executable(cpr_demo main.cpp)
target_link_libraries(cpr_demo ${CONAN_LIBS})
