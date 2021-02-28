cmake_minimum_required(VERSION 3.16)

project(gameboy_emulator LANGUAGES CXX)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTS "Build tests and add them to ctest" ON)


# TODO: Add/remove options - check which options are useful
add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_library(instructions_8bit instructions_8bit.cc)
add_library(instructions_16bit instructions_16bit.cc)

add_library(instructions instructions.cc)
target_link_libraries(instructions
	instructions_8bit
	instructions_16bit
)

add_library(cpu cpu.cc)
target_link_libraries(cpu
	instructions
)

add_executable(gameboy main.cc)
target_link_libraries(gameboy
	cpu
)

add_executable(debugger debugger.cc)
find_package(curses)
target_link_libraries(debugger
	ncurses
	cpu
)


if (${BUILD_TESTS})
	enable_testing()
	add_subdirectory("tests/")
endif()
