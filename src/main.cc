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
		display.update(emu.get_memory(), cycles);
		// std::cout << "INFO: instructions executed: " << std::dec << counter << '\n';

		if (frame_cycles > CYCLES_PER_FRAME) {
			std::cout << "INFO: main: calling render, frame_cycles " << frame_cycles << ", CYCLES_PER_FRAME " << CYCLES_PER_FRAME << "\n";
			frame_cycles -= CYCLES_PER_FRAME;

			const auto continue_running = display.render(emu.get_memory());
			if (!continue_running) {
				return 0;
			}
			emu.get_memory().direct_write(0xff44, 0);

			const auto end = std::chrono::high_resolution_clock::now();
			const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			++frames;
			++frames10s;

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

		++counter;
	}

	return 0;
}
