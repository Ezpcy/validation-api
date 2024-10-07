#!/usr/bin/env bash

# Check if build/Release and build/Debug exists
if [ ! -d "build/Release" ]; then
  conan install . -s build_type=Release --build=missing
elif [ ! -d "build/Debug" ]; then
  conan install . -s build_type=Debug --build=missing
fi

# Get argument if it's passed
if [ "$1" == "r" ]; then
  cd build/Release
  cmake ../.. -DCMAKE_BUILD_TYPE=Release
  cmake --build .
  ./validation-api
else 
  cd build/Debug
  cmake ../.. -DCMAKE_BUILD_TYPE=Debug
  cmake --build .
  ./validation-api
fi


