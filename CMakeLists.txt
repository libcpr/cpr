cmake_minimum_required(VERSION 2.8.7)

# Allow use of project folders for IDEs like Visual Studio, so we
# could organize projects into relevant folders: "cpr", "tests" & "external (libraries)".
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "CMake")

project(cpr)

set(CPR_MAJOR_VERSION 0)
set(CPR_MINOR_VERSION 1)
set(CPR_PATCH_VERSION 0)
set(CPR_VERSION ${CPR_MAJOR_VERSION}.${CPR_MINOR_VERSION}.${CPR_PATCH_VERSION})


if(NOT ${CMAKE_VERSION} LESS 3.2)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
else()
    message(STATUS "Checking compiler flags for C++11 support.")
    # Set C++11 support flags for various compilers
    include(CheckCXXCompilerFlag)
    check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)
    check_cxx_compiler_flag("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
    if(COMPILER_SUPPORTS_CXX11)
        message(STATUS "C++11 is supported.")
        if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
        endif()
    elseif(COMPILER_SUPPORTS_CXX0X)
        message(STATUS "C++0x is supported.")
        if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -stdlib=libc++")
        else()
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
        endif()
    else()
        message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
    endif()
endif()

#set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
#set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
    set(DEF_INSTALL_CMAKE_DIR CMake)
else()
    set(DEF_INSTALL_CMAKE_DIR lib/CMake/cpr)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

# set up include-directories
include_directories("${PROJECT_BINARY_DIR}/${CMAKE_PROJECT_NAME}"   # to find cpr_export.h
                    ${PROJECT_BINARY_DIR})                          # to find cpr/config.h


# Library names    
set(CPR_LIBRARIES cpr CACHE INTERNAL "")
set(CPR_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include CACHE INTERNAL "")

macro(cpr_option OPTION_NAME OPTION_TEXT OPTION_DEFAULT)
    option(${OPTION_NAME} ${OPTION_TEXT} ${OPTION_DEFAULT})
    if(DEFINED ENV{${OPTION_NAME}})
        # Allow setting the option through an environment variable
        set(${OPTION_NAME} $ENV{${OPTION_NAME}})
    endif()
    if(${OPTION_NAME})
        add_definitions(-D${OPTION_NAME})
    endif()
    message(STATUS "  ${OPTION_NAME}: ${${OPTION_NAME}}")
endmacro()

message(STATUS "C++ Requests CMake Options")
message(STATUS "=======================================================")
cpr_option(USE_SYSTEM_CURL
    "If ON, this project will look in the system paths for an installed curl library" OFF)
cpr_option(BUILD_CPR_TESTS "Set to ON to build cpr tests." ON)
cpr_option(INSECURE_CURL "Set to ON to disable verification of SSL certificates." OFF)
cpr_option(GENERATE_COVERAGE "Set to ON to generate coverage reports." OFF)
cpr_option(USE_SYSTEM_GTEST
    "If ON, this project will look in the system paths for an installed gtest library" OFF)
message(STATUS "=======================================================")

if(BUILD_CPR_TESTS)
    enable_testing()
endif()

add_subdirectory(opt)
add_subdirectory(cpr)
if(BUILD_CPR_TESTS) 
    # add_subdirectory(test)
endif()


# The interesting stuff goes here
# ===============================

# Add all targets to the build-tree export set
export(TARGETS cpr FILE "${PROJECT_BINARY_DIR}/cpr-targets.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE cpr)

# Create the cpr-config.cmake and cpr-config-version files
file(RELATIVE_PATH REL_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/CMake ${CMAKE_INSTALL_PREFIX}/include)
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(cpr-config.cmake.in "${PROJECT_BINARY_DIR}/cpr-config.cmake" @ONLY)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${CPR_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file( cpr-config.cmake.in
                "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/cpr-config.cmake" @ONLY)
# ... for both
configure_file( cpr-config-version.cmake.in
                "${PROJECT_BINARY_DIR}/cpr-config-version.cmake" @ONLY)

# Install the cpr-config.cmake and cpr-config-version.cmake
install(FILES
        "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/cpr-config.cmake"
        "${PROJECT_BINARY_DIR}/cpr-config-version.cmake"
        DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

# Install the export set for use with the install-tree
install(EXPORT cpr-targets DESTINATION
        "${INSTALL_CMAKE_DIR}" COMPONENT dev)
