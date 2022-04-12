cmake_minimum_required(VERSION 3.16)

project(gameboy_emulator LANGUAGES CXX)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# TODO: Add/remove options - check which options are useful
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

option(BUILD_TESTS "Build tests and add them to ctest" ON)

add_subdirectory("src/")

if (${BUILD_TESTS})
	enable_testing()
	add_subdirectory("tests/")
endif()

