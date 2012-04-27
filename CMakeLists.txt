cmake_minimum_required(VERSION 2.4.6)

project(xbow_400)

# set the default path for built executables to the "bin" directory
IF(NOT EXECUTABLE_OUTPUT_PATH)
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
ENDIF(NOT EXECUTABLE_OUTPUT_PATH)
# set the default path for built libraries to the "lib" directory
IF(NOT LIBRARY_OUTPUT_PATH)
    set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
ENDIF(NOT LIBRARY_OUTPUT_PATH)

# add the include folder to the include path
include_directories(${PROJECT_SOURCE_DIR}/include)

# Find the Serial library
IF(NOT serial_FOUND)
    find_package(serial REQUIRED)
ENDIF(NOT serial_FOUND)

# Check that serial was found
IF(NOT serial_FOUND)
    message(FATAL_ERROR "Serial library not found or provided, failing.")
ENDIF(NOT serial_FOUND)

include_directories(${serial_INCLUDE_DIRS})

# Find Boost
find_package(Boost COMPONENTS system filesystem thread REQUIRED)

link_directories(${Boost_LIBRARY_DIRS})
include_directories(${Boost_INCLUDE_DIRS})

# Compile the xbow_400 Library
add_library(xbow_400 src/xbow_400.cpp include/xbow_400.h)
target_link_libraries(xbow_400 ${serial_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY})

# Compile the Test program
add_executable(test_xbow_400 src/test_xbow_400.cpp)
# Link the Test program to the Serial library
target_link_libraries(test_xbow_400 xbow_400)

# Configure make install
IF(NOT CMAKE_INSTALL_PREFIX)
    SET(CMAKE_INSTALL_PREFIX /usr/local)
ENDIF(NOT CMAKE_INSTALL_PREFIX)

INSTALL(TARGETS xbow_400
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)

INSTALL(FILES include/xbow_400.h include/xbow_400_types.h DESTINATION include)

IF(NOT CMAKE_FIND_INSTALL_PATH)
    set(CMAKE_FIND_INSTALL_PATH ${CMAKE_ROOT})
ENDIF(NOT CMAKE_FIND_INSTALL_PATH)

# INSTALL(FILES Findxbow_400.cmake DESTINATION ${CMAKE_FIND_INSTALL_PATH}/Modules/)

ADD_CUSTOM_TARGET (uninstall @echo uninstall package)

IF (UNIX)
  ADD_CUSTOM_COMMAND(
    COMMENT "uninstall package"
    COMMAND xargs ARGS rm < install_manifest.txt

    TARGET  uninstall
  )
ELSE(UNIX)
  ADD_CUSTOM_COMMAND(
    COMMENT "uninstall only implemented in unix"
    TARGET  uninstall
  )
ENDIF(UNIX)
