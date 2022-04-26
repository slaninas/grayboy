cmake_minimum_required(VERSION 3.16)

add_library(test_main test_main.cc)
include_directories("../src/")

add_executable(registers_tests  registers_tests.cc)
target_link_libraries(registers_tests test_main)

add_executable(registers_snapshot_tests  registers_snapshot_tests.cc)
target_link_libraries(registers_snapshot_tests test_main)

add_executable(cpu_utils_tests  cpu_utils_tests.cc)
target_link_libraries(cpu_utils_tests test_main)

add_test("registers_tests " registers_tests)
add_test("registers_snapshot_tests " registers_snapshot_tests)
add_test("cpu_utils_tests" cpu_utils_tests)
