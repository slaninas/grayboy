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
	auto display = Display{};
	auto frame_cycles = uint64_t{0};

	auto frames = uint64_t{0};
	auto frames10s = uint64_t{0};

	auto start = std::chrono::high_resolution_clock::now();
	auto start10s = std::chrono::high_resolution_clock::now();

	while (1) {
		const auto cycles = emu.execute_next();
		frame_cycles += cycles;

		display.update(emu.get_memory(), cycles);

		if (frame_cycles >= CYCLES_PER_FRAME) {
			frame_cycles -= CYCLES_PER_FRAME;
			if (!display.render(emu.get_memory())) {
				return 0;
			}
		++frames;
		++frames10s;
		}

		const auto end = std::chrono::high_resolution_clock::now();
		const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

		if (diff >= 1000) {
				  std::cout << "INFO: FPS: " << (frames / (1000.0 / diff)) << '\n';
				  start = std::chrono::high_resolution_clock::now();
				  frames = 0;
		}

		const auto end10s = std::chrono::high_resolution_clock::now();
		const auto diff10s = std::chrono::duration_cast<std::chrono::milliseconds>(end10s - start10s).count();

		if (diff10s >= 10'000) {
				  std::cout << "INFO: 10s average FPS: " << (frames10s / (10'000.0 / diff10s)) / 10 << '\n';
				  start10s = std::chrono::high_resolution_clock::now();
				  frames10s = 0;
		}

	}

	return 0;
}
