#pragma once

#include "cartridge.h"
#include "cpu.h"

class Emulator {
public:
	Emulator(const std::string& cartridge_path) {

		const auto rom = Cartridge{cartridge_path}.dump();

		auto array = Memory::ArrayType{};

		std::transform(begin(rom), end(rom), begin(array), [](const auto& el) { return static_cast<uint8_t>(el); });

		// TODO: Remove, for debug purposes only
		array[0xff44] = 0x90;

		const auto regs =
			RegistersChanger{.AF = 0x01b0, .BC=0x0013, .DE = 0x00d8, .HL = 0x014d, .PC = 0x0100, .SP = 0xfffe}.get(Registers{});

		cpu_ = Cpu{array, regs};
		raw_dump(cpu_.memory_dump(), "init_memory_dump");
	}

	Emulator(const std::string& bootrom_path, const std::string& cartridge_path) {

		const auto boot_rom = Cartridge{bootrom_path}.dump();
		const auto rom = Cartridge{cartridge_path}.dump();

		auto array = Memory::ArrayType{};

		std::transform(begin(boot_rom), end(boot_rom), begin(array), [](const auto& el) { return static_cast<uint8_t>(el); });
		std::transform(begin(rom) + 0x100, end(rom), begin(array) + 0x100, [](const auto& el) { return static_cast<uint8_t>(el); });

		// TODO: Remove, for debug purposes only
		array[0xff44] = 0x90;

		const auto regs =
			Registers{};

		cpu_ = Cpu{array, regs};
		raw_dump(cpu_.memory_dump(), "init_memory_dump");
	}

	auto execute_next() -> uint64_t {
		return cpu_.execute_next();
	}


private:
	Cpu cpu_ = Cpu{};

};
