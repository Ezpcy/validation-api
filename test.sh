#!/usr/bin/env bash

cd build/Debug
cmake ../.. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build .

cd tests

if [[ "$1" == "conf" ]]; then
  ./tests --gtest_filter=ConfigServiceTest.*
elif [[ "$1" == "val" ]]; then
  ./tests --gtest_filter=ValidationTest.*
elif [[ "$1" == "watch" ]];then
  ./tests --gtest_filter=FileWatcherTest.*
elif [[ "$1" == "server" ]];then
  ./tests --gtest_filter=ValidationServerTest.*
fi
