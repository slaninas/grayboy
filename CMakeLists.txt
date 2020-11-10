cmake_minimum_required(VERSION 3.16)

project(gameboy_emulator LANGUAGES CXX)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)

option(BUILD_TESTS "Build tests and add them to ctest" ON)


# TODO: Add/remove options - check which options are useful
add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(gameboy main.cc)

add_executable(debugger debugger.cc)
find_package(curses)
target_link_libraries(debugger ncurses)


if (${BUILD_TESTS})
	enable_testing()
	add_subdirectory("tests/")
endif()
