cmake_minimum_required(VERSION 3.16)

add_library(test_main test_main.cc)
include_directories("..")

add_executable(registers_tests  registers_tests.cc)
target_link_libraries(registers_tests test_main)

add_executable(memory_tests  memory_tests.cc)
target_link_libraries(memory_tests test_main)

add_executable(cpu_utils_tests  cpu_utils_tests.cc)
target_link_libraries(cpu_utils_tests test_main)

add_executable(cpu_ld_tests  cpu_ld_tests.cc)
target_link_libraries(cpu_ld_tests test_main)

add_executable(cpu_misc_tests  cpu_misc_tests.cc)
target_link_libraries(cpu_misc_tests test_main)

add_executable(cpu_arithmetic_tests  cpu_arithmetic_tests.cc)
target_link_libraries(cpu_arithmetic_tests test_main)

add_executable(cpu_bit_operation_tests  cpu_bit_operation_tests.cc)
target_link_libraries(cpu_bit_operation_tests test_main)

add_test("registers_tests " registers_tests)
add_test("memory_tests " memory_tests)
add_test("cpu_utils_tests" cpu_utils_tests)
add_test("cpu_ld_tests" cpu_ld_tests)
add_test("cpu_misc_tests" cpu_misc_tests)
add_test("cpu_arithmetic_tests" cpu_arithmetic_tests)
add_test("cpu_bit_operation_tests" cpu_bit_operation_tests)
