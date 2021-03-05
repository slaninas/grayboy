#include "cartridge.h"
#include "cpu.h"
#include "registers.h"

#include <array>
#include <iostream>
#include <vector>

auto main(int argc, const char** argv) -> int
{
	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " cartridge_filename\n";
		return 1;
	}

	auto filename = std::string(argv[1]);

	auto cart = Cartridge{filename};
	// cart.print_info();

	const auto rom = cart.dump();
	auto array = Memory::ArrayType{};

	std::transform(begin(rom), end(rom), begin(array), [](const auto& el) { return static_cast<uint8_t>(el); });

	// Initializing values same way bgb emualtor does it
	const auto regs =
	  RegistersChanger{.AF = 0x1180, .DE = 0xff56, .HL = 0x000d, .PC = 0x0100, .SP = 0xfffe}.get(Registers{});

	auto cpu = Cpu{array, regs};

	// while (1) {
	// [[maybe_unused]] const auto cycles = cpu.execute_next();
	// }
	//
	check_implemented();
}
