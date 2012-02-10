## Project Setup
cmake_minimum_required(VERSION 2.4.6)

project(xbow400)

## Configurations

option(XBOW400_BUILD_EXAMPLES "To build or not to build." OFF)

# Set the default path for built executables to the "bin" directory
IF(NOT DEFINED(EXECUTABLE_OUTPUT_PATH))
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
ENDIF(NOT DEFINED(EXECUTABLE_OUTPUT_PATH))
# set the default path for built libraries to the "lib" directory
IF(NOT DEFINED(LIBRARY_OUTPUT_PATH))
    set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
ENDIF(NOT DEFINED(LIBRARY_OUTPUT_PATH))

## Configure the build system

# Find serial
find_package(serial REQUIRED)

# Add the include folder to the include path
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${serial_INCLUDE_DIRS})

# Add default source files
set(XBOW400_SRCS src/xbow400.cc)
# Add default header files
set(XBOW400_HEADERS include/xbow400/xbow400.h)

## Build the Serial Library

# Compile the Library
add_library(xbow400 ${XBOW400_SRCS})
target_link_libraries(xbow400 ${serial_LIBRARIES})

## Build Examples

# If asked to
IF(XBOW400_BUILD_EXAMPLES)
    # Compile the Serial Test program
    add_executable(xbow400_example examples/xbow400_example.cc)
    # Link the Test program to the xbow400 library
    target_link_libraries(xbow400_example xbow400)
ENDIF(XBOW400_BUILD_EXAMPLES)
