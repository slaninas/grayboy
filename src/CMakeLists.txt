cmake_minimum_required(VERSION 3.16)

add_library(instructions instructions.cc)

add_library(cpu cpu.cc)
target_link_libraries(cpu
	instructions
)

find_package(PkgConfig)

PKG_SEARCH_MODULE(SDL2 REQUIRED sdl2)
add_library(emulator emulator.h)
target_link_libraries(emulator
	cpu
	${pkgcfg_lib_SDL2_SDL2}
)


add_executable(grayboy main.cc)
target_link_libraries(grayboy
	emulator
)

# add_executable(debugger debugger.cc)
# find_package(curses)
# target_link_libraries(debugger
	# ncurses
	# emulator
	# ${pkgcfg_lib_SDL2_SDL2}
# )
