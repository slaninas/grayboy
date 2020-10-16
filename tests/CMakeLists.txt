cmake_minimum_required(VERSION 3.16)

add_library(test_main test_main.cc)
include_directories("..")

add_executable(cpu_ld_tests  cpu_ld_tests.cc)
target_link_libraries(cpu_ld_tests test_main)
add_test("cpu_ld_tests " cpu_ld_tests)
