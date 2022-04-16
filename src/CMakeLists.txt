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

find_package(PkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
add_library(emulator emulator.cc)
target_link_libraries(emulator
	cpu
)


add_executable(grayboy main.cc)
target_link_libraries(grayboy
	cpu
	${pkgcfg_lib_SDL2_SDL2}
)

add_executable(debugger debugger.cc)
find_package(curses)
target_link_libraries(debugger
	ncurses
	cpu
)
