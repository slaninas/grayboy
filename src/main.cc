#include "emulator.h"

#include <array>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>


// TODO: At the end run all tests again to check whether the fixes for tests didn't break previous
// State (BLARGG comparison)
// ************************
// - BootRom has no diff compared to https://github.com/wheremyfoodat/Gameboy-logs except last 100 lines but it should be just checksum (and I'm running different cartridge
//   than the author of those dumps
// - Blargg 01 works fine
// - Blargg 03 works fine
// - Blargg 04 works fine
// - Blargg 05 works fine
// - Blargg 06 works fine (PC must be set to 0x101 instead of 0x100 at the beginning)
// - Blargg 07 prints that test passed but there are differences with the Gameboy-logs - it seems the logs have RST instruction merged with following instructions
// - Blargg 08 works fine
// - Blargg 09 works fine
// - Blargg 10 works fine
// - Blargg 11 works fine

auto main(int argc, const char** argv) -> int
{
	if (argc < 2 || argc > 3) {
		std::cout << "Usage: " << argv[0] << " <boot_rom> cartridge_filename\n";
		return 1;
	}

	auto emu = argc == 2 ? Emulator{argv[1]} : Emulator{argv[1], argv[2]};


	auto counter = static_cast<uint64_t>(1);
	auto total_cycles = static_cast<uint64_t>(0);

	const auto start = std::chrono::high_resolution_clock::now();
	const auto display_update = 1'000'000 / 16;
	auto display_counter = display_update;
	while (1) {

		const auto cycles = emu.execute_next();
		total_cycles += cycles;
		// std::cout << "INFO: instructions executed: " << std::dec << counter << '\n';

		if (++display_counter > display_update) {
			display_counter -= display_update;
			if (!emu.update_display()) {
				return 0;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		if (counter > 500'000'000) {
		// if (counter > 200'000) {
			const auto stop = std::chrono::high_resolution_clock::now();
			const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
			std::cout << "\nINFO: total_cycles " << total_cycles << ", duration " << duration.count() << '\n';
			std::cout << emu.get_serial_link() << '\n';
			break;
		}
		++counter;
	}
	//
	// check_implemented();
}
