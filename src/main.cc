#include "display.h"
#include "emulator.h"

#include <array>
#include <iostream>
#include <thread>
#include <vector>

auto main(int argc, const char** argv) -> int
{
	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " cartridge_filename\n";
		return 1;
	}

	auto emu = Emulator<false>{argv[1]};
	emu.run();

	return 0;
}
