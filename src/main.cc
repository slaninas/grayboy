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
	auto start = std::chrono::high_resolution_clock::now();
	auto start10s = std::chrono::high_resolution_clock::now();
	auto frames = 0;
	auto frames10s = 0;
	while (1) {

		const auto cycles = emu.execute_next();
		total_cycles += cycles;
		frame_cycles += cycles;
		if (!display.update(emu.get_memory(), cycles)) {
			return 0;
		}
	}

	return 0;
}
