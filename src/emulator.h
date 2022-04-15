#pragma once

#include "cartridge.h"
#include "cpu.h"

class Timer {
public:
	const uint64_t CPU_FREQUENCY = 4'194'304 / 4;
	const uint64_t DIV_REGISTER_FREQUENCY = 16'384;
	const uint64_t DIV_REGISTER_CYCLES_PER_UPDATE = CPU_FREQUENCY / DIV_REGISTER_FREQUENCY;

	// Inspiration: http://emudev.de/gameboy-emulator/interrupts-and-timers/
	auto update(Memory& memory, const uint64_t& new_cycles) {
		div_register_cycles_ += new_cycles;

		if (div_register_cycles_ > DIV_REGISTER_CYCLES_PER_UPDATE) {
			div_register_cycles_ = div_register_cycles_ % DIV_REGISTER_CYCLES_PER_UPDATE;
			memory.write(0xff04, memory.read(0xff04) + 1);
		}

		const auto TAC = memory.read(0xff07);
		if (TAC & 0x4) {
			timer_counter_cycles_ += new_cycles;

			auto frequency = 4096;

			if ((TAC & 0x3) == 0x0) {
				frequency = frequency * 1;
			}
			else if ((TAC & 0x3) == 0x1) {
				frequency = frequency * 64;
			}
			else if ((TAC & 0x3) == 0x2) {
				frequency = frequency * 16;
			}
			else if ((TAC & 0x3) == 0x3) {
				frequency = frequency * 4;
			}

			const auto timer_counter_cycles_per_update = CPU_FREQUENCY / frequency;
			while (timer_counter_cycles_ >= timer_counter_cycles_per_update) {
				memory.write(0xff05, memory.read(0xff05) + 1);

				if (memory.read(0xff05) == 0x00) {
					memory.write(0xff0f, memory.read(0xff0f) | 0x4);

					const auto TMA = memory.read(0xff06);
					memory.write(0xff05, TMA);
				}

				timer_counter_cycles_ -= timer_counter_cycles_per_update;
			}

		}


	}

private:
	uint64_t div_register_cycles_ = 0;
	uint64_t timer_counter_cycles_ = 0;
};

class Emulator {
public:
	Emulator(const std::string& cartridge_path) {

		const auto rom = Cartridge{cartridge_path}.dump();

		auto array = Memory::ArrayType{};

		std::transform(begin(rom), end(rom), begin(array), [](const auto& el) { return static_cast<uint8_t>(el); });

		// TODO: Remove, for debug purposes only
		array[0xff44] = 0x90;

		// TODO: Init the RAM as well somehow? Check boot rom
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

		// Interupts
		if (cpu_.registers().read_IME()) {
			auto& mem = cpu_.get_memory();
			if (mem.read(0xffff) & 0x1 && mem.read(0xff0f) & 0x1) {
				// V-Blank
				return handle_interupt(0x1);

			} else if (mem.read(0xffff) & 0x2 && mem.read(0xff0f) & 0x2) {
				// LCD Stat
				return handle_interupt(0x2);

			} else if (mem.read(0xffff) & 0x4 && mem.read(0xff0f) & 0x4) {
				// Timer
				return handle_interupt(0x4);

			} else if (mem.read(0xffff) & 0x8 && mem.read(0xff0f) & 0x8) {
				// Serial
				return handle_interupt(0x8);

			} else if (mem.read(0xffff) & 0x16 && mem.read(0xff0f) & 0x16) {
				// Joypad
				return handle_interupt(0x16);

			}

		}

		const auto cycles = cpu_.execute_next();

		const auto ff02 = cpu_.get_memory().read(0xff02);

		if (ff02 == 0x81) {
			const auto c = static_cast<char>(cpu_.get_memory().read(0xff01));
			// std::cout << c;
			serial_link_ += c;
			cpu_.get_memory().write(0xff02, 0x80);
		}

		timer_.update(cpu_.get_memory(), cycles);

		return cycles;
	}

	auto execute_instructions(const uint64_t& count) {
		for (auto i = static_cast<uint64_t>(0); i < count; ++i) {
			execute_next();
		}
	}

	auto get_serial_link() const -> const std::string& {
		return serial_link_;
	}

	auto dump_memory(const std::string& filename) -> void {
		raw_dump(cpu_.memory_dump(), filename);
	}


private:

	// TODO: Unit test?
	auto handle_interupt(const uint8_t& bit) -> uint64_t {
		auto& mem = cpu_.get_memory();
		mem.write(0xff0f, mem.read(0xff0f) ^ bit);

		auto& regs = cpu_.registers();

		const auto PC = regs.read("PC");
		const auto SP = regs.read("SP");

		const auto return_address_high = static_cast<uint8_t>((PC & 0xff00) >> 8);
		const auto return_address_low = static_cast<uint8_t>(PC & 0x00ff);

		mem.write(SP - 1, return_address_high);
		mem.write(SP - 2, return_address_low);
		regs.write("SP", SP -2);

		switch (bit) {
			case 0x1:
				regs.write("PC", 0x40);
				break;
			case 0x2:
				regs.write("PC", 0x48);
				break;
			case 0x4:
				regs.write("PC", 0x50);
				break;
			case 0x8:
				regs.write("PC", 0x58);
				break;
			case 0x16:
				regs.write("PC", 0x60);
				break;
		}

		return 5;
	}

	Cpu cpu_ = Cpu{};
	std::string serial_link_ = {};
	Timer timer_ = {};
};
