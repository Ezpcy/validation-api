#!/usr/bin/env bash

cd build/Debug
cmake ../.. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=generators/conan_toolchain.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build .

cd tests
./tests --gtest_filter=ConfigServiceTest.CreatingConfig
