cmake_minimum_required(VERSION 3.16)

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

add_library(emulator emulator.cc)
target_link_libraries(emulator
	cpu
)

add_executable(grayboy main.cc)
target_link_libraries(grayboy
	cpu
)

add_executable(debugger debugger.cc)
find_package(curses)
target_link_libraries(debugger
	ncurses
	cpu
)
