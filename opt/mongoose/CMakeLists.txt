project(mongoose C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mongoose)
add_library(mongoose
    mongoose/mongoose.c)