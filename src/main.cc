#include "cartridge.h"
#include "cpu.h"
#include "registers.h"

#include <array>
#include <iostream>
#include <vector>


// State
// ************************
// - BootRom has no diff compared to https://github.com/wheremyfoodat/Gameboy-logs except last 100 lines but it should be just checksum (and I'm running different cartridge
//   than the author of those dumps

auto main(int argc, const char** argv) -> int
{
	if (argc != 3) {
		std::cout << "Usage: " << argv[0] << " boot_rom cartridge_filename\n";
		return 1;
	}

	const auto boot = Cartridge{std::string(argv[1])};
	const auto cart = Cartridge{std::string(argv[2])};
	// cart.print_info();

	const auto boot_rom = boot.dump();
	const auto rom = cart.dump();

	auto array = Memory::ArrayType{};

	std::transform(begin(boot_rom), end(boot_rom), begin(array), [](const auto& el) { return static_cast<uint8_t>(el); });
	std::transform(begin(rom) + 0x100, end(rom), begin(array) + 0x100, [](const auto& el) { return static_cast<uint8_t>(el); });

	// TODO: Remove, for debug purposes only
	array[0xff44] = 0x90;


	// Initializing values same way bgb emualtor does it
	// RegistersChanger{.AF = 0x01b0, .BC=0x0013, .DE = 0x00d8, .HL = 0x014d, .PC = 0x0100, .SP = 0xfffe}.get(Registers{});
	const auto regs =
		Registers{};

	auto cpu = Cpu{array, regs};
	raw_dump(cpu.memory_dump(), "init_memory_dump");


	// auto counter = static_cast<uint64_t>(0);
	while (1) {
		[[maybe_unused]] const auto cycles = cpu.execute_next();
		std::cout << "instructions executed: " << counter++ << '\n';
	}
	//
	// check_implemented();
}
