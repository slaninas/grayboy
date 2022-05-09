cmake_minimum_required(VERSION 3.16)

add_library(instructions instructions.cc)

add_library(cpu cpu.cc)
target_link_libraries(cpu
	instructions
)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_library(emulator emulator.h)

target_link_libraries(emulator
	cpu
	${SDL2_LIBRARIES}
)

add_executable(grayboy main.cc)
target_link_libraries(grayboy
	emulator
)
