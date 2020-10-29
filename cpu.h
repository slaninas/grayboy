#pragma once

#include <vector>
#include <iostream>
#include <sstream>
#include <cassert>

#include "registers.h"
#include "memory.h"

struct Instruction {
	std::string mnemonic;
	uint16_t opcode;
	uint8_t size;
	std::function<uint8_t(Registers&, Memory&, const uint16_t&)> run;

	auto operator()(Registers& regs, Memory& mem, const uint16_t& PC) const {
		const auto cycles = run(regs, mem, PC);
		regs.write("PC", PC + size);
		return cycles;
	}
};

// TODO: Merge (half) carries somehow?
// TODO: Are (half) carries correct?
// Detect half-carry for addition, see https://robdor.com/2016/08/10/gameboy-emulator-half-carry-flag/
[[nodiscard]] auto half_carry_add_8bit(const uint16_t a, const uint16_t b) {
	return (((a & 0xf) + (b & 0xf)) & 0x10) > 0;
}
[[nodiscard]] auto half_carry_add_16bit(const uint16_t a, const uint16_t b) {
	return (((a & 0x0fff) + (b & 0x0fff)) & 0x1000) > 0;
}

[[nodiscard]] auto carry_add_8bit(const uint16_t a, const uint16_t b) {
	return (((a & 0xff) + (b & 0xff)) & 0x100) > 0;
}
[[nodiscard]] auto carry_add_16bit(const uint16_t a, const uint16_t b) {
	return (((a & 0xffff) + (b & 0xffff)) & 0x10000) > 0;
}

// https://www.reddit.com/r/EmuDev/comments/4clh23/trouble_with_halfcarrycarry_flag/
[[nodiscard]] auto half_carry_sub_8bit(const uint16_t a, const uint16_t b) {
	return ((a & 0x0f) - (b & 0x0f)) < 0;
}

[[nodiscard]] auto half_carry_sub_16bit(const uint16_t a, const uint16_t b) {
	return ((a & 0x0fff) - (b & 0xfff)) < 0;
}

[[nodiscard]] auto carry_sub_8bit(const uint16_t a, const uint16_t b) {
	return ((a & 0xff) - (b & 0xff)) < 0;
}
[[nodiscard]] auto carry_sub_16bit(const uint16_t a, const uint16_t b) {
	return ((a & 0xffff) - (b & 0xffff)) < 0;
}

template<size_t kSize>
// TODO: remove _fn from name
void instruction_inc_fn(const char(&reg_name)[kSize], Registers& regs) {
	constexpr auto reg_name_size = kSize - 1; // Subtract \0 at the end
	// 8bit
	if constexpr (reg_name_size == 1) {
		const auto old_value = regs.read(reg_name);
		const auto new_value = static_cast<uint8_t>(old_value + 1);
		regs.write(reg_name, new_value);

		regs.set_flag("Z", new_value == 0x00);
		regs.set_flag("N", false);
		regs.set_flag("H", half_carry_add_8bit(old_value, 1));
	}
	// 16bit
	else {
		assert(false && "Not implemented yet");
	}
}

template<size_t kSize>
void instruction_dec_fn(const char(&reg_name)[kSize], Registers& regs) {
	constexpr auto reg_name_size = kSize - 1; // Subtract \0 at the end
	// 8bit
	if constexpr (reg_name_size == 1) {
		const auto old_value = regs.read(reg_name);
		const auto new_value = static_cast<uint8_t>(old_value - 1);
		regs.write(reg_name, new_value);

		regs.set_flag("Z", new_value == 0x00);
		regs.set_flag("N", true);
		regs.set_flag("H", half_carry_sub_8bit(old_value, 1));
	}
	// 16bit
	else {
		assert(false && "Not implemented yet");
	}
}

// Make exceptions asserts and run in debug
// TODO: Add unit tests
class Cpu {
public:
	using MemoryType = Memory;

	Cpu() = default;

	// TODO: Make move possible?
	Cpu(const MemoryType& memory) :
		memory_{memory}
	{}
	Cpu(const MemoryType& memory, const Registers& regs) :
		memory_{memory},
		regs_{regs}
	{}


	void clear_registers() {
		regs_.clear();
	}



	[[nodiscard]] auto execute_next() {
		const auto PC = regs_.read("PC");
		const auto opcode = memory_.read(PC);

		if (opcode == 0xBC) {
			throw std::runtime_error("16-bit opcodes not implemented yet.");
		}

		const auto instruction = find_by_opcode(opcode);
		const auto cycles = instruction(regs_, memory_, PC);
		return cycles;
	}

	[[nodiscard]] auto registers_dump() const {
		return regs_;
	}

	[[nodiscard]] auto memory_dump() const {
		return memory_.dump();
	}

	[[nodiscard]] auto& registers() {
		return regs_;
	}


private:
	MemoryType memory_ = {};
	Registers regs_ = {};

	// See https://meganesulli.com/generate-gb-opcodes/
	std::vector<Instruction> instructions_ = {
		// TODO: {"STOP", 0x10, 2, 1},
		// TODO: JR NZ, s8 - 0x20
		// TODO: JR NC, s8 - 0x30
		// TODO: DAA - 0x27
		{"NOP", 0x00, 1,
			[]([[maybe_unused]] auto& regs, [[maybe_unused]] auto& mem, [[maybe_unused]] const auto& PC) {
				return 1;
			}
		},

		// Load 16bit value into 16bit register
		{"LD BC, d16", 0x01, 3,
			[](auto& regs, auto& memory, const auto& PC) {
				regs.write("C", memory.read(PC + 1));
				regs.write("B", memory.read(PC + 2));
				return 3;
			}
		},
		{"LD DE, d16", 0x11, 3,
			[](auto& regs, auto& memory, const auto& PC) {
				regs.write("E", memory.read(PC + 1));
				regs.write("D", memory.read(PC + 2));
				return 3;
			}
		},
		{"LD HL, d16", 0x21, 3,
			[](auto& regs, auto& memory, const auto& PC) {
				regs.write("L", memory.read(PC + 1));
				regs.write("H", memory.read(PC + 2));
				return 3;
			}
		},
		{"LD SP, d16", 0x31, 3,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto value = static_cast<uint16_t>(memory.read(PC + 1) + (memory.read(PC + 2) << 8));
				regs.write("SP", value);
				return 3;
			}
		},

		// Load A into an address stored in given register
		{"LD (BC), A", 0x02, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				memory.write(regs.read("BC"), regs.read("A"));
				return 2;
			}
		},
		{"LD (DE), A", 0x12, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				memory.write(regs.read("DE"), regs.read("A"));
				return 2;
			}
		},
		{"LD (HL+), A", 0x22, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				memory.write(regs.read("HL"), regs.read("A"));
				regs.write("HL", regs.read("HL") + 1);
				return 2;
			}
		},
		{"LD (HL-), A", 0x32, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				memory.write(regs.read("HL"), regs.read("A"));
				regs.write("HL", regs.read("HL") - 1);
				return 2;
			}
		},

		// Increment 16bit register
		{"INC BC", 0x03, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("BC", regs.read("BC") + 1);
				return 2;
			}
		},
		{"INC DE", 0x13, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("DE", regs.read("DE") + 1);
				return 2;
			}
		},
		{"INC HL", 0x23, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("HL", regs.read("HL") + 1);
				return 2;
			}
		},
		{"INC SP", 0x33, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("SP", regs.read("SP") + 1);
				return 2;
			}
		},

		// 8bit increment
		{"INC B", 0x04, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_inc_fn("B", regs);
				return 1;
			}
		},
		{"INC C", 0x0c, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_inc_fn("C", regs);
				return 1;
			}
		},
		{"INC D", 0x14, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_inc_fn("D", regs);
				return 1;
			}
		},
		{"INC H", 0x24, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_inc_fn("H", regs);
				return 1;
			}
		},
		{"INC (HL)", 0x34, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				// TODO: Use instruction_inc method?
					const auto address = regs.read("HL");
					const auto old_value = memory.read(address);
					const auto new_value = static_cast<uint8_t>(old_value + 1);
					memory.write(address, new_value);

					regs.set_flag("Z", new_value == 0x00);
					regs.set_flag("N", false);
					regs.set_flag("H", half_carry_add_8bit(old_value, 1));
				return 3;
			}
		},

		// Decrement 8bit
		{"DEC B", 0x05, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_dec_fn("B", regs);
				return 1;
			}
		},
		{"DEC C", 0x0d, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_dec_fn("C", regs);
				return 1;
			}
		},
		{"DEC D", 0x15, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_dec_fn("D", regs);
				return 1;
			}
		},
		{"DEC H", 0x25, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_dec_fn("H", regs);
				return 1;
			}
		},
		{"DEC (HL)", 0x35, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				// TODO: Use instruction_dec function?
				const auto address = regs.read("HL");
				const auto old_value = memory.read(address);
				const auto new_value = static_cast<uint8_t>(old_value - 1);
				memory.write(address, new_value);

				regs.set_flag("Z", new_value == 0x00);
				regs.set_flag("N", true);
				regs.set_flag("H", half_carry_sub_8bit(old_value, 1));
				return 3;
			}
		},

		// Load into 8bit
		{"LD B, d8", 0x06, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				regs.write("B", memory.read(PC + 1));
				return 2;
			}
		},
		{"LD D, d8", 0x16, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				regs.write("D", memory.read(PC + 1));
				return 2;
			}
		},
		{"LD H, d8", 0x26, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				regs.write("H", memory.read(PC + 1));
				return 2;
			}
		},
		// https://meganesulli.com/generate-gb-opcodes/ says it's 1 byte long instruction but it doesn't make sense, right?
		// TODO: Is 3 cycles correct?
		{"LD (HL), d8", 0x36, 1,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto address = regs.read("HL");
				const auto value = memory.read(PC + 1);
				memory.write(address, value);
				return 3;
			}
		},

		{"LD A, (BC)", 0x0a, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("BC");
				regs.write("A", memory.read(address));
				return 2;
			}
		},
		{"LD C, d8", 0x0e, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto value = memory.read(PC + 1);
				regs.write("C", value);
				return 2;
			}
		},

		// Decrement 16bit
		{"DEC BC", 0x0b, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("BC", regs.read("BC") - 1);
				return 2;
			}
		},

		// Other instructions, not grouped (yet)
		{"RLCA", 0x07, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				// TODO: Put into method
				const auto A = regs.read("A");
				const auto msb = (A & (1 << 7)) >> 7;
				const auto A_new = static_cast<uint8_t>((A << 1) + msb);
				regs.write("A", A_new);

				regs.set_flag("Z", false);
				regs.set_flag("N", false);
				regs.set_flag("H", false);
				regs.set_flag("C", static_cast<bool>(msb));
				return 1;
			}
		},
		{"RLA", 0x17, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				// TODO: Put into method
				const auto A = regs.read("A");
				const auto msb = (A & (1 << 7)) >> 7;
				const auto A_new = static_cast<uint8_t>((A << 1) + regs.read_flag("C")); // Setting C here as bit 0, it's only difference from RLCA which uses msb
				regs.write("A", A_new);

				regs.set_flag("Z", false);
				regs.set_flag("N", false);
				regs.set_flag("H", false);
				regs.set_flag("C", static_cast<bool>(msb));
				return 1;
			}
		},
		{"RRCA", 0x0f, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				// TODO: Put into method
				const auto A = regs.read("A");
				const auto lsb = (A & (1 << 0)) >> 0;
				const auto A_new = static_cast<uint8_t>((A >> 1) + (lsb << 7));
				regs.write("A", A_new);

				regs.set_flag("Z", false);
				regs.set_flag("N", false);
				regs.set_flag("H", false);
				regs.set_flag("C", static_cast<bool>(lsb));
				return 1;
			}
		},
		{"LD (a16), SP", 0x08, 3,
			[](auto& regs, auto& memory, const auto& PC) {
				// TODO: Put into method
				const auto address = static_cast<uint16_t>((memory.read(PC + 1) << 8) + memory.read(PC + 2));
				const auto SP = regs.read("SP");
				memory.write(address, static_cast<uint8_t>(SP & 0x00ff));
				memory.write(address + 1, static_cast<uint8_t>((SP & 0xff00) >> 8));
				return 5;
			}
		},
		{"SCF", 0x37, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.set_flag("C", true);
				return 1;
			}
		},
		// -------------------- End of ungrouped instructions ----------------------

		// Add 16bit
		{"Add HL, BC", 0x09, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
					// TODO: Put into method
					const auto BC = regs.read("BC");
					const auto HL_old = regs.read("HL");
					const auto HL_new = static_cast<uint16_t>(HL_old + BC);

					regs.write("HL", HL_new);

					regs.set_flag("N", 0);
					regs.set_flag("H", half_carry_add_16bit(HL_old, BC));
					regs.set_flag("C", carry_add_16bit(HL_old, BC));
				return 2;
			}
		},

	};

	[[nodiscard]] const Instruction& find_by_opcode(const uint16_t opcode) {
		auto res = std::find_if(begin(instructions_), end(instructions_), [opcode](const auto& instruction) { return instruction.opcode ==  opcode; });
		if (res == end(instructions_)) {
			auto stream = std::stringstream{};
			stream << std::hex << opcode;
			throw std::runtime_error("Opcode 0x" + stream.str() + " not found.");
		}
		return *res;
	}

};
