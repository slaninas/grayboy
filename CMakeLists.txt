cmake_minimum_required(VERSION 3.16)

project(FILL_NAME_HERE LANGUAGES CXX)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 20)

# TODO: Add/remove options - check which options are useful
add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(gameboy  main.cc)
