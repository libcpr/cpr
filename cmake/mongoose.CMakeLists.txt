cmake_minimum_required(VERSION 3.15)
project(mongoose C)


add_library(mongoose STATIC mongoose.c)
target_include_directories(mongoose PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(ENABLE_SSL_TESTS)
    # Enable mongoose SSL
    target_compile_definitions(mongoose PUBLIC MG_ENABLE_SSL)
    target_link_libraries(mongoose PRIVATE OpenSSL::SSL)
endif()
