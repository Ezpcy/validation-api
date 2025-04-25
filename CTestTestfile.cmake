# CMake generated Testfile for 
# Source directory: /home/ezpz/Nextcloud/DevStuff/cpp/validation-api
# Build directory: /home/ezpz/Nextcloud/DevStuff/cpp/validation-api
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(LoggerTest "/home/ezpz/Nextcloud/DevStuff/cpp/validation-api/tests/tests" "--gtest_filter=LoggerTest.*")
set_tests_properties(LoggerTest PROPERTIES  _BACKTRACE_TRIPLES "/home/ezpz/Nextcloud/DevStuff/cpp/validation-api/CMakeLists.txt;119;add_test;/home/ezpz/Nextcloud/DevStuff/cpp/validation-api/CMakeLists.txt;0;")
add_test(ConfigWatcherTest "/home/ezpz/Nextcloud/DevStuff/cpp/validation-api/tests/tests" "--gtest_filter=ConfigWatcherTest.*")
set_tests_properties(ConfigWatcherTest PROPERTIES  _BACKTRACE_TRIPLES "/home/ezpz/Nextcloud/DevStuff/cpp/validation-api/CMakeLists.txt;122;add_test;/home/ezpz/Nextcloud/DevStuff/cpp/validation-api/CMakeLists.txt;0;")
add_test(ConfigServiceTest "/home/ezpz/Nextcloud/DevStuff/cpp/validation-api/tests/tests" "--gtest_filter=ConfigServiceTest.*")
set_tests_properties(ConfigServiceTest PROPERTIES  _BACKTRACE_TRIPLES "/home/ezpz/Nextcloud/DevStuff/cpp/validation-api/CMakeLists.txt;126;add_test;/home/ezpz/Nextcloud/DevStuff/cpp/validation-api/CMakeLists.txt;0;")
add_test(ValidationTest "/home/ezpz/Nextcloud/DevStuff/cpp/validation-api/tests/tests" "--gtest_filter=ValidationTest.*")
set_tests_properties(ValidationTest PROPERTIES  _BACKTRACE_TRIPLES "/home/ezpz/Nextcloud/DevStuff/cpp/validation-api/CMakeLists.txt;130;add_test;/home/ezpz/Nextcloud/DevStuff/cpp/validation-api/CMakeLists.txt;0;")
add_test(ValidationServerTest "/home/ezpz/Nextcloud/DevStuff/cpp/validation-api/tests/tests" "--gtest_filter=ValidationServerTest.*")
set_tests_properties(ValidationServerTest PROPERTIES  _BACKTRACE_TRIPLES "/home/ezpz/Nextcloud/DevStuff/cpp/validation-api/CMakeLists.txt;133;add_test;/home/ezpz/Nextcloud/DevStuff/cpp/validation-api/CMakeLists.txt;0;")
