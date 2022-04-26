#include "emulator.h"

#include <array>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

#include "display.h"


auto main(int argc, const char** argv) -> int
{
	const uint64_t CPU_FREQUENCY = 4'194'304 / 4;
	const uint64_t CYCLES_PER_FRAME = CPU_FREQUENCY / 60;

	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " cartridge_filename\n";
		return 1;
	}

	auto emu = Emulator{argv[1]};


	auto counter = static_cast<uint64_t>(1);
	auto frame_cycles = static_cast<uint64_t>(0);
	auto total_cycles = static_cast<uint64_t>(0);

	auto display = Display{};
	while (1) {

		const auto cycles = emu.execute_next();
		total_cycles += cycles;
		frame_cycles += cycles;
		display.update(emu.get_memory(), cycles);
		// std::cout << "INFO: instructions executed: " << std::dec << counter << '\n';

		if (frame_cycles > CYCLES_PER_FRAME) {
			frame_cycles -= CYCLES_PER_FRAME;

			if (!display.render(emu.get_memory())) {
				return 0;
			}
		}

		++counter;
	}

	return 0;
}
