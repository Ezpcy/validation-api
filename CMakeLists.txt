cmake_minimum_required(VERSION 3.15)
project(validation-api)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

# Set build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)

# Set Boost multithreading support
set(Boost_USE_MULTITHREAD ON)

# set the toolchain based on the build type
# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#   set(CMAKE_TOOLCHAIN_FILE "${CMAKE_BINARY_DIR}/Debug/generators/conan_toolchain.cmake")
# elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
#   set(CMAKE_TOOLCHAIN_FILE "${CMAKE_BINARY_DIR}/Release/generators/conan_toolchain.cmake")
# endif()


# Find Boost, spdlog, and gtest components
find_package(Boost REQUIRED)
find_package(spdlog REQUIRED)
find_package(GTest REQUIRED)

# Add the executable
add_executable(validation-api src/main.cpp)

# Link libraries
target_include_directories(validation-api PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(validation-api PRIVATE Boost::boost spdlog::spdlog)

# Test setup
set(SOURCES
    ${CMAKE_SOURCE_DIR}/src/Logger.cpp
)

# Add your test files
set(TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/tests/LoggerTest.cpp
)

add_executable(LoggerTest ${TEST_SOURCES} ${SOURCES})

set_target_properties(LoggerTest PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

target_link_libraries(LoggerTest PRIVATE gtest::gtest spdlog::spdlog Boost::boost)
add_test(NAME LoggerTest COMMAND LoggerTest)


