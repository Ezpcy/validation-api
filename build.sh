#!/usr/bin/env bash

conan install . -s build_type=Release --build=missing
conan install . -s build_type=Debug --build=missing

# Get argument if it's passed
if [ "$1" == "r" ]; then
  cd build/Release
  cmake ../.. -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
  cmake --build .
  ./validation-api
else 
  cd build/Debug
  cmake ../.. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
  cmake --build .
  ./validation-api example
fi




