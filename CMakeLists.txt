cmake_minimum_required(VERSION 3.15)
project(validation-api)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include_directories(${CMAKE_SOURCE_DIR}/include)

option(BUILD_TESTS "Build test targets" OFF)

message(STATUS "Build tests ${BUILD_TESTS}")

# Set build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE
      "Debug"
      CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_FLAGS_RELEASE
    "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=x86-64 -flto -funroll-loops")

# If building for production server for older cpu architecture and the above
# doesn't work, try one the below set(CMAKE_CXX_FLAGS_RELEASE
# "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=sandybridge -mtune=generic -flto
# -funroll-loops") set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3
# -march=nehalem -mtune=generic -mno-avx -flto -funroll-loops")

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXE_LINKER_FLAGS "-static")

# Set Boost multithreading support
set(Boost_USE_MULTITHREAD ON)

# # set the toolchain based on the build type if(CMAKE_BUILD_TYPE STREQUAL
# "Debug") set(CMAKE_TOOLCHAIN_FILE
# "${CMAKE_BINARY_DIR}/Debug/generators/conan_toolchain.cmake")
# elseif(CMAKE_BUILD_TYPE STREQUAL "Release") set(CMAKE_TOOLCHAIN_FILE
# "${CMAKE_BINARY_DIR}/Release/generators/conan_toolchain.cmake") endif()

# Find Boost, spdlog, and gtest components
find_package(Boost CONFIG)
find_package(spdlog REQUIRED)
find_package(GTest REQUIRED)
find_package(pugixml REQUIRED)
find_package(nlohmann_json REQUIRED)

# Add the executable
add_executable(
  validation-api
  src/main.cpp
  src/Logger.cpp
  src/ConfigWatcher.cpp
  src/ConfigService.cpp
  src/ValidationServer.cpp
  src/lib/Helpers.cpp
  src/lib/Validation.cpp)

# Copy config
add_custom_target(
  copy_server_json ALL
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/server.json
          ${CMAKE_BINARY_DIR}/server.json
  COMMENT "Copying server.json to build directory")

# Link libraries
target_include_directories(validation-api PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(
  validation-api
  PRIVATE fmt::fmt
          Boost::boost
          spdlog::spdlog
          Boost::filesystem
          Boost::system
          Boost::thread
          pugixml::pugixml
          nlohmann_json::nlohmann_json)

# Test setup
if(BUILD_TESTS)
  enable_testing()
  set(SOURCES
      ${CMAKE_SOURCE_DIR}/src/Logger.cpp
      ${CMAKE_SOURCE_DIR}/src/ConfigWatcher.cpp
      ${CMAKE_SOURCE_DIR}/src/ConfigService.cpp
      ${CMAKE_SOURCE_DIR}/src/ValidationServer.cpp
      ${CMAKE_SOURCE_DIR}/src/lib/Helpers.cpp
      ${CMAKE_SOURCE_DIR}/src/lib/Validation.cpp)

  # Add your test files
  set(TEST_SOURCES
      ${CMAKE_SOURCE_DIR}/tests/LoggerTest.cpp
      ${CMAKE_SOURCE_DIR}/tests/ConfigWatcherTest.cpp
      ${CMAKE_SOURCE_DIR}/tests/ConfigServiceTest.cpp
      ${CMAKE_SOURCE_DIR}/tests/ValidationServerTest.cpp
      ${CMAKE_SOURCE_DIR}/tests/ValidationTest.cpp)

  add_executable(tests ${TEST_SOURCES} ${SOURCES})

  set_target_properties(tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY
                                         ${CMAKE_BINARY_DIR}/tests)

  target_link_libraries(
    tests
    PRIVATE fmt::fmt
            gtest::gtest
            spdlog::spdlog
            Boost::boost
            Boost::filesystem
            Boost::system
            Boost::thread
            pugixml::pugixml
            nlohmann_json::nlohmann_json)
  # For LoggerTest suite
  add_test(NAME LoggerTest COMMAND tests --gtest_filter=LoggerTest.*)

  # For ConfigWatcherTest suite
  add_test(NAME ConfigWatcherTest COMMAND tests
                                          --gtest_filter=ConfigWatcherTest.*)

  # For ConfigServiceTest suite
  add_test(NAME ConfigServiceTest COMMAND tests
                                          --gtest_filter=ConfigServiceTest.*)

  # For ValidationTest suite
  add_test(NAME ValidationTest COMMAND tests --gtest_filter=ValidationTest.*)

  # For ServerTest suite
  add_test(NAME ValidationServerTest
           COMMAND tests --gtest_filter=ValidationServerTest.*)
endif()
