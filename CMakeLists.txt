cmake_minimum_required(VERSION 3.16)

project(gameboy_emulator LANGUAGES CXX)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
enable_testing()

# TODO: Add/remove options - check which options are useful
add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(gameboy  main.cc)

add_subdirectory("tests/")
