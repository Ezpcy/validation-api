# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /nix/store/yzi080r2c1zn2jzrhcfdv7dmr92yw07l-cmake-3.29.6/bin/cmake

# The command to remove a file.
RM = /nix/store/yzi080r2c1zn2jzrhcfdv7dmr92yw07l-cmake-3.29.6/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api

# Include any dependencies generated for this target.
include CMakeFiles/tests.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/tests.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/tests.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tests.dir/flags.make

CMakeFiles/tests.dir/tests/LoggerTest.cpp.o: CMakeFiles/tests.dir/flags.make
CMakeFiles/tests.dir/tests/LoggerTest.cpp.o: tests/LoggerTest.cpp
CMakeFiles/tests.dir/tests/LoggerTest.cpp.o: CMakeFiles/tests.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/tests.dir/tests/LoggerTest.cpp.o"
	/nix/store/4rz4z2bkb68vwbdxcwq0jxh2fyhhiqkh-clang-wrapper-18.1.8/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/tests.dir/tests/LoggerTest.cpp.o -MF CMakeFiles/tests.dir/tests/LoggerTest.cpp.o.d -o CMakeFiles/tests.dir/tests/LoggerTest.cpp.o -c /home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api/tests/LoggerTest.cpp

CMakeFiles/tests.dir/tests/LoggerTest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/tests.dir/tests/LoggerTest.cpp.i"
	/nix/store/4rz4z2bkb68vwbdxcwq0jxh2fyhhiqkh-clang-wrapper-18.1.8/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api/tests/LoggerTest.cpp > CMakeFiles/tests.dir/tests/LoggerTest.cpp.i

CMakeFiles/tests.dir/tests/LoggerTest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/tests.dir/tests/LoggerTest.cpp.s"
	/nix/store/4rz4z2bkb68vwbdxcwq0jxh2fyhhiqkh-clang-wrapper-18.1.8/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api/tests/LoggerTest.cpp -o CMakeFiles/tests.dir/tests/LoggerTest.cpp.s

CMakeFiles/tests.dir/tests/ConfigWatcherTest.cpp.o: CMakeFiles/tests.dir/flags.make
CMakeFiles/tests.dir/tests/ConfigWatcherTest.cpp.o: tests/ConfigWatcherTest.cpp
CMakeFiles/tests.dir/tests/ConfigWatcherTest.cpp.o: CMakeFiles/tests.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/tests.dir/tests/ConfigWatcherTest.cpp.o"
	/nix/store/4rz4z2bkb68vwbdxcwq0jxh2fyhhiqkh-clang-wrapper-18.1.8/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/tests.dir/tests/ConfigWatcherTest.cpp.o -MF CMakeFiles/tests.dir/tests/ConfigWatcherTest.cpp.o.d -o CMakeFiles/tests.dir/tests/ConfigWatcherTest.cpp.o -c /home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api/tests/ConfigWatcherTest.cpp

CMakeFiles/tests.dir/tests/ConfigWatcherTest.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/tests.dir/tests/ConfigWatcherTest.cpp.i"
	/nix/store/4rz4z2bkb68vwbdxcwq0jxh2fyhhiqkh-clang-wrapper-18.1.8/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api/tests/ConfigWatcherTest.cpp > CMakeFiles/tests.dir/tests/ConfigWatcherTest.cpp.i

CMakeFiles/tests.dir/tests/ConfigWatcherTest.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/tests.dir/tests/ConfigWatcherTest.cpp.s"
	/nix/store/4rz4z2bkb68vwbdxcwq0jxh2fyhhiqkh-clang-wrapper-18.1.8/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api/tests/ConfigWatcherTest.cpp -o CMakeFiles/tests.dir/tests/ConfigWatcherTest.cpp.s

CMakeFiles/tests.dir/src/Logger.cpp.o: CMakeFiles/tests.dir/flags.make
CMakeFiles/tests.dir/src/Logger.cpp.o: src/Logger.cpp
CMakeFiles/tests.dir/src/Logger.cpp.o: CMakeFiles/tests.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/tests.dir/src/Logger.cpp.o"
	/nix/store/4rz4z2bkb68vwbdxcwq0jxh2fyhhiqkh-clang-wrapper-18.1.8/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/tests.dir/src/Logger.cpp.o -MF CMakeFiles/tests.dir/src/Logger.cpp.o.d -o CMakeFiles/tests.dir/src/Logger.cpp.o -c /home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api/src/Logger.cpp

CMakeFiles/tests.dir/src/Logger.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/tests.dir/src/Logger.cpp.i"
	/nix/store/4rz4z2bkb68vwbdxcwq0jxh2fyhhiqkh-clang-wrapper-18.1.8/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api/src/Logger.cpp > CMakeFiles/tests.dir/src/Logger.cpp.i

CMakeFiles/tests.dir/src/Logger.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/tests.dir/src/Logger.cpp.s"
	/nix/store/4rz4z2bkb68vwbdxcwq0jxh2fyhhiqkh-clang-wrapper-18.1.8/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api/src/Logger.cpp -o CMakeFiles/tests.dir/src/Logger.cpp.s

CMakeFiles/tests.dir/src/ConfigWatcher.cpp.o: CMakeFiles/tests.dir/flags.make
CMakeFiles/tests.dir/src/ConfigWatcher.cpp.o: src/ConfigWatcher.cpp
CMakeFiles/tests.dir/src/ConfigWatcher.cpp.o: CMakeFiles/tests.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/tests.dir/src/ConfigWatcher.cpp.o"
	/nix/store/4rz4z2bkb68vwbdxcwq0jxh2fyhhiqkh-clang-wrapper-18.1.8/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/tests.dir/src/ConfigWatcher.cpp.o -MF CMakeFiles/tests.dir/src/ConfigWatcher.cpp.o.d -o CMakeFiles/tests.dir/src/ConfigWatcher.cpp.o -c /home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api/src/ConfigWatcher.cpp

CMakeFiles/tests.dir/src/ConfigWatcher.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/tests.dir/src/ConfigWatcher.cpp.i"
	/nix/store/4rz4z2bkb68vwbdxcwq0jxh2fyhhiqkh-clang-wrapper-18.1.8/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api/src/ConfigWatcher.cpp > CMakeFiles/tests.dir/src/ConfigWatcher.cpp.i

CMakeFiles/tests.dir/src/ConfigWatcher.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/tests.dir/src/ConfigWatcher.cpp.s"
	/nix/store/4rz4z2bkb68vwbdxcwq0jxh2fyhhiqkh-clang-wrapper-18.1.8/bin/clang++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api/src/ConfigWatcher.cpp -o CMakeFiles/tests.dir/src/ConfigWatcher.cpp.s

# Object files for target tests
tests_OBJECTS = \
"CMakeFiles/tests.dir/tests/LoggerTest.cpp.o" \
"CMakeFiles/tests.dir/tests/ConfigWatcherTest.cpp.o" \
"CMakeFiles/tests.dir/src/Logger.cpp.o" \
"CMakeFiles/tests.dir/src/ConfigWatcher.cpp.o"

# External object files for target tests
tests_EXTERNAL_OBJECTS =

tests/tests: CMakeFiles/tests.dir/tests/LoggerTest.cpp.o
tests/tests: CMakeFiles/tests.dir/tests/ConfigWatcherTest.cpp.o
tests/tests: CMakeFiles/tests.dir/src/Logger.cpp.o
tests/tests: CMakeFiles/tests.dir/src/ConfigWatcher.cpp.o
tests/tests: CMakeFiles/tests.dir/build.make
tests/tests: /home/ezpz/.conan2/p/b/gtest5f539f11c177b/p/lib/libgmock_main.a
tests/tests: /home/ezpz/.conan2/p/b/gtest5f539f11c177b/p/lib/libgmock.a
tests/tests: /home/ezpz/.conan2/p/b/gtest5f539f11c177b/p/lib/libgtest_main.a
tests/tests: /home/ezpz/.conan2/p/b/gtest5f539f11c177b/p/lib/libgtest.a
tests/tests: /home/ezpz/.conan2/p/b/spdlo82da955bfb1c4/p/lib/libspdlogd.a
tests/tests: /home/ezpz/.conan2/p/b/fmt7bcf00f37b3ef/p/lib/libfmtd.a
tests/tests: /home/ezpz/.conan2/p/b/boostfbccd637683d3/p/lib/libboost_filesystem.a
tests/tests: /home/ezpz/.conan2/p/b/boostfbccd637683d3/p/lib/libboost_thread.a
tests/tests: /home/ezpz/.conan2/p/b/boostfbccd637683d3/p/lib/libboost_atomic.a
tests/tests: /home/ezpz/.conan2/p/b/boostfbccd637683d3/p/lib/libboost_chrono.a
tests/tests: /home/ezpz/.conan2/p/b/boostfbccd637683d3/p/lib/libboost_container.a
tests/tests: /home/ezpz/.conan2/p/b/boostfbccd637683d3/p/lib/libboost_date_time.a
tests/tests: /home/ezpz/.conan2/p/b/boostfbccd637683d3/p/lib/libboost_exception.a
tests/tests: /home/ezpz/.conan2/p/b/pugixf34cdf4af5557/p/lib/libpugixml.a
tests/tests: CMakeFiles/tests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Linking CXX executable tests/tests"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tests.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tests.dir/build: tests/tests
.PHONY : CMakeFiles/tests.dir/build

CMakeFiles/tests.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tests.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tests.dir/clean

CMakeFiles/tests.dir/depend:
	cd /home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api /home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api /home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api /home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api /home/ezpz/Nextcloud/NextcloudData/DevStuff/cpp/validation-api/CMakeFiles/tests.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/tests.dir/depend
