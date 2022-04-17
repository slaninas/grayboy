#include "emulator.h"

#include <array>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

#include "display.h"


// State (BLARGG comparison)
// ************************
// - BootRom has no diff compared to https://github.com/wheremyfoodat/Gameboy-logs except last 100 lines but it should be just checksum (and I'm running different cartridge
//   than the author of those dumps
// - Blargg 01 works fine
// - Blargg 02 works fine
// - Blargg 03 works fine
// - Blargg 04 works fine
// - Blargg 05 works fine
// - Blargg 06 works fine
// - Blargg 07 works fine
// - Blargg 08 works fine
// - Blargg 09 works fine
// - Blargg 10 works fine
// - Blargg 11 works fine


auto main(int argc, const char** argv) -> int
{
	const uint64_t CPU_FREQUENCY = 4'194'304 / 4;
	const uint64_t CYCLES_PER_FRAME = CPU_FREQUENCY / 60;

	if (argc < 2 || argc > 3) {
		std::cout << "Usage: " << argv[0] << " <boot_rom> cartridge_filename\n";
		return 1;
	}

	auto emu = argc == 2 ? Emulator{argv[1]} : Emulator{argv[1], argv[2]};


	auto counter = static_cast<uint64_t>(1);
	auto frame_cycles = static_cast<uint64_t>(0);
	auto total_cycles = static_cast<uint64_t>(0);

	auto display = Display{};
	auto start = std::chrono::high_resolution_clock::now();
	while (1) {

		const auto cycles = emu.execute_next();
		total_cycles += cycles;
		frame_cycles += cycles;
		display.update(emu.get_memory(), cycles);
		// std::cout << "INFO: instructions executed: " << std::dec << counter << '\n';

		if (frame_cycles > CYCLES_PER_FRAME) {
			frame_cycles -= CYCLES_PER_FRAME;

			const auto end = std::chrono::high_resolution_clock::now();
			const auto diff = duration_cast<std::chrono::milliseconds>(end - start).count();

			std::this_thread::sleep_for(std::chrono::milliseconds{static_cast<int>(1000.0/60.0 -diff)});
			if (!display.render(emu.get_memory())) {
				return 0;
			}

			start = std::chrono::high_resolution_clock::now();
		}

		++counter;
	}
	//
	// check_implemented();
}
