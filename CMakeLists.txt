cmake_minimum_required(VERSION 3.15)
project(validation-api)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


# Set build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" force)
endif()

# set the toolchain based on the build type
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_TOOLCHAIN_FILE "${CMAKE_BINARY_DIR}/Debug/generators/conan_toolchain.cmake")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_TOOLCHAIN_FILE "${CMAKE_BINARY_DIR}/Release/generators/conan_toolchain.cmake")
endif()


# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# For static libraries (.a files)
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# For shared libraries (.so or .dll files)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Output directory for executables
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Set Boost multithreading support
set(Boost_USE_MULTITHREAD ON)

# Find Boost components (adjust components as needed, asio and system in this case)
find_package(Boost REQUIRED)

# Add the executable
add_executable(validation-api src/main.cpp)

# Link Boost libraries
target_link_libraries(validation-api Boost::boost) 


