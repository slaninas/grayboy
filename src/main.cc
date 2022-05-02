#include "emulator.h"

#include <array>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

#include "display.h"


auto main(int argc, const char** argv) -> int
{
	// const uint64_t CPU_FREQUENCY = 4'194'304 / 4;

	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " cartridge_filename\n";
		return 1;
	}

	auto emu = Emulator{argv[1]};
	auto display = Display{};

	while (1) {
		const auto cycles = emu.execute_next();
		if (!display.update(emu.get_memory(), cycles)) {
			return 0;
		}
	}

	return 0;
}
