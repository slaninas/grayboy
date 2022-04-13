#include "emulator.h"

#include <array>
#include <iostream>
#include <vector>


// TODO: At the end run all tests again to check whether the fixes for tests didn't break previous
// State (BLARGG comparison)
// ************************
// - BootRom has no diff compared to https://github.com/wheremyfoodat/Gameboy-logs except last 100 lines but it should be just checksum (and I'm running different cartridge
//   than the author of those dumps
// - Blargg 03 works fine
// - Blargg 04 works fine
// - Blargg 05 works fine
// - Blargg 06 works fine (PC must be set to 0x101 instead of 0x100 at the beginning)
// - Blargg 07 prints that test passed but there are differences with the Gameboy-logs - it seems the logs have RST instruction merged with following instructions

auto main(int argc, const char** argv) -> int
{
	if (argc < 2 || argc > 3) {
		std::cout << "Usage: " << argv[0] << " boot_rom cartridge_filename\n";
		return 1;
	}

	auto emu = argc == 2 ? Emulator{argv[1]} : Emulator{argv[1], argv[2]};


	auto counter = static_cast<uint64_t>(0);
	while (1) {
		[[maybe_unused]] const auto cycles = emu.execute_next();
		std::cout << "instructions executed: " << counter++ << '\n';
	}
	//
	// check_implemented();
}
