project(mongoose C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mongoose)
add_library(mongoose mongoose/mongoose.c)

if(CMAKE_USE_OPENSSL)
    # Enable mongoose SSL
    target_compile_definitions(mongoose PUBLIC MG_ENABLE_SSL)
    target_link_libraries(mongoose ${OPENSSL_LIBRARIES})
endif()