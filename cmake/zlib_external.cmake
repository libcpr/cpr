# ZLIB

# Fix Windows missing "zlib.dll":
if(WIN32 AND (${CMAKE_PROJECT_NAME} STREQUAL ${PROJECT_NAME}))
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/$<CONFIG> CACHE INTERNAL "" FORCE)
endif()

set(ZLIB_COMPAT ON CACHE INTERNAL "" FORCE)
set(BUILD_TESTING OFF CACHE INTERNAL "" FORCE)

FetchContent_Declare(zlib
                    URL https://github.com/zlib-ng/zlib-ng/archive/2.3.3.tar.gz
                    URL_HASH SHA256=f9c65aa9c852eb8255b636fd9f07ce1c406f061ec19a2e7d508b318ca0c907d1
                    USES_TERMINAL_DOWNLOAD TRUE)
FetchContent_MakeAvailable(zlib)

# Fix Windows zlib dll names from "zlibd1.dll" to "zlib.dll":
if(WIN32 AND BUILD_SHARED_LIBS)
    set_target_properties(zlib PROPERTIES OUTPUT_NAME "zlib")
    set_target_properties(zlib PROPERTIES DEBUG_POSTFIX "")
    set_target_properties(zlib PROPERTIES SUFFIX ".dll")
endif()
