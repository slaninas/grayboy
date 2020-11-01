#pragma once

#include <vector>
#include <iostream>
#include <sstream>
#include <cassert>
#include <type_traits>

#include "registers.h"
#include "memory.h"

struct Instruction {
	std::string mnemonic;
	uint16_t opcode;
	uint8_t size;
	std::function<uint8_t(Registers&, Memory&, const uint16_t&)> run;

	[[nodiscard]] auto operator()(Registers& regs, Memory& mem, const uint16_t& PC) const {
		const auto cycles = run(regs, mem, PC);
		const auto PC_new = regs.read("PC");
		regs.write("PC", PC_new + size);
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


template<size_t kDestSize, size_t kSecondRegNameSize>
void instruction_add(const char(&dest_name)[kDestSize], const char(&second_reg_name)[kSecondRegNameSize], Registers& regs) {
	static_assert(kDestSize == kSecondRegNameSize, "Add for 8bit + 16bit or vice versa not implemented.");
	constexpr auto real_size = kDestSize - 1;

	// 8bit
	if constexpr (real_size == 1) {
		const auto second_reg = regs.read(second_reg_name);
		const auto dest_old = regs.read(dest_name);
		const auto dest_new = static_cast<decltype(second_reg)>(dest_old + second_reg);

		regs.write(dest_name, dest_new);

		regs.set_flag("Z", dest_new == 0); // TODO: This is the only difference between 8 and 16bit, merge rest?
		regs.set_flag("N", 0);
		regs.set_flag("H", half_carry_add_16bit(dest_old, second_reg));
		regs.set_flag("C", carry_add_16bit(dest_old, second_reg));
	}
	// 16bit
	else {
		const auto second_reg = regs.read(second_reg_name);
		const auto dest_old = regs.read(dest_name);
		const auto dest_new = static_cast<decltype(second_reg)>(dest_old + second_reg);

		regs.write(dest_name, dest_new);

		regs.set_flag("N", 0);
		regs.set_flag("H", half_carry_add_16bit(dest_old, second_reg));
		regs.set_flag("C", carry_add_16bit(dest_old, second_reg));
	}
}

// TODO: Unit unit test for this
template<size_t kDestSize, typename ValueType>
void instruction_addc(const char(&dest_name)[kDestSize], const ValueType& value, Registers& regs) {
	constexpr auto real_size = kDestSize - 1;
	static_assert(real_size == 1, "Only 8bit add with carry supported.");
	static_assert(std::is_same_v<ValueType, uint8_t>, "Only 8bit add with carry supported.");

	const auto C = regs.read_flag("C");
	const auto value_with_carry = static_cast<uint8_t>(value + C);
	auto half_carry = half_carry_add_8bit(value, C);
	auto carry = carry_add_8bit(value, C);

	const auto dest_old = regs.read(dest_name);
	const auto dest_new = static_cast<uint8_t>(dest_old + value_with_carry);
	half_carry |= half_carry_add_8bit(dest_old, value_with_carry);
	carry |= carry_add_8bit(dest_old, value_with_carry);

	regs.write(dest_name, dest_new);
	regs.set_flag("Z", dest_new == 0);
	regs.set_flag("N", false);
	regs.set_flag("H", half_carry);
	regs.set_flag("C", carry);
}

template<size_t kDestSize, size_t kSecondRegNameSize>
void instruction_addc(const char(&dest_name)[kDestSize], const char(&second_reg_name)[kSecondRegNameSize], Registers& regs) {
	instruction_addc(dest_name, regs.read(second_reg_name), regs);
}

// TODO: Unit unit test for this
template<size_t kDestSize, typename ValueType>
void instruction_sub(const char(&dest_name)[kDestSize], const ValueType& value, Registers& regs) {
	constexpr auto real_size = kDestSize - 1;
	static_assert(real_size == 1, "Only 8bit sub is supported.");
	static_assert(std::is_same_v<ValueType, uint8_t>, "Only 8bit sub is supported.");

	const auto dest_old = regs.read(dest_name);
	const auto dest_new = dest_old - value;

	regs.write(dest_name, dest_new);
	regs.set_flag("Z", dest_new == 0);
	regs.set_flag("N", true);
	regs.set_flag("H", half_carry_sub_8bit(dest_old, value));
	regs.set_flag("C", carry_sub_8bit(dest_old, value));
}

// TODO: Unit unit test for this
template<size_t kDestSize, size_t kSecondRegNameSize>
void instruction_sub(const char(&dest_name)[kDestSize], const char(&second_reg_name)[kSecondRegNameSize], Registers& regs) {
	instruction_sub(dest_name, regs.read(second_reg_name), regs);
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
		// TODO: HALT - 0x76
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
		{"INC E", 0x1c, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_inc_fn("E", regs);
				return 1;
			}
		},
		{"INC L", 0x2c, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_inc_fn("L", regs);
				return 1;
			}
		},
		{"INC A", 0x3c, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_inc_fn("A", regs);
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
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
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
		{"DEC E", 0x1d, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_dec_fn("E", regs);
				return 1;
			}
		},
		{"DEC L", 0x2d, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_dec_fn("L", regs);
				return 1;
			}
		},
		{"DEC A", 0x3d, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_dec_fn("A", regs);
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
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
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
		{"LD A, (DE)", 0x1a, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("DE");
				regs.write("A", memory.read(address));
				return 2;
			}
		},
		{"LD A, (HL+)", 0x2a, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				regs.write("A", memory.read(address));
				regs.write("HL", address + 1);
				return 2;
			}
		},
		{"LD A, (HL-)", 0x3a, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				regs.write("A", memory.read(address));
				regs.write("HL", address - 1);
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
		{"LD E, d8", 0x1e, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto value = memory.read(PC + 1);
				regs.write("E", value);
				return 2;
			}
		},
		{"LD L, d8", 0x2e, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto value = memory.read(PC + 1);
				regs.write("L", value);
				return 2;
			}
		},
		{"LD A, d8", 0x3e, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto value = memory.read(PC + 1);
				regs.write("A", value);
				return 2;
			}
		},
		{"LD B, B", 0x40, 1,
			[]([[maybe_unused]] auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				return 1;
			}
		},
		{"LD B, C", 0x41, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("B", regs.read("C"));
				return 1;
			}
		},
		{"LD B, D", 0x42, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("B", regs.read("D"));
				return 1;
			}
		},
		{"LD B, E", 0x43, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("B", regs.read("E"));
				return 1;
			}
		},
		{"LD B, H", 0x44, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("B", regs.read("H"));
				return 1;
			}
		},
		{"LD B, L", 0x45, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("B", regs.read("L"));
				return 1;
			}
		},
		{"LD B, (HL)", 0x46, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				regs.write("B", memory.read(address));
				return 2;
			}
		},
		{"LD B, A", 0x47, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("B", regs.read("A"));
				return 1;
			}
		},
		{"LD C, B", 0x48, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("C", regs.read("B"));
				return 1;
			}
		},
		{"LD C, C", 0x49, 1,
			[]([[maybe_unused]]auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				return 1;
			}
		},
		{"LD C, D", 0x4a, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("C", regs.read("D"));
				return 1;
			}
		},
		{"LD C, E", 0x4b, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("C", regs.read("E"));
				return 1;
			}
		},
		{"LD C, H", 0x4c, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("C", regs.read("H"));
				return 1;
			}
		},
		{"LD C, L", 0x4d, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("C", regs.read("L"));
				return 1;
			}
		},
		{"LD C, (HL)", 0x4e, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				regs.write("C", memory.read(address));
				return 2;
			}
		},
		{"LD C, A", 0x4f, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("C", regs.read("A"));
				return 1;
			}
		},
		{"LD D, B", 0x50, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("D", regs.read("B"));
				return 1;
			}
		},
		{"LD D, C", 0x51, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("D", regs.read("C"));
				return 1;
			}
		},
		{"LD D, D", 0x52, 1,
			[]([[maybe_unused]] auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				return 1;
			}
		},
		{"LD D, E", 0x53, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("D", regs.read("E"));
				return 1;
			}
		},
		{"LD D, H", 0x54, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("D", regs.read("H"));
				return 1;
			}
		},
		{"LD D, L", 0x55, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("D", regs.read("L"));
				return 1;
			}
		},
		{"LD D, (HL)", 0x56, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				regs.write("D", memory.read(address));
				return 2;
			}
		},
		{"LD D, A", 0x57, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("D", regs.read("A"));
				return 1;
			}
		},
		{"LD E, B", 0x58, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("E", regs.read("B"));
				return 1;
			}
		},
		{"LD E, C", 0x59, 1,
			[]([[maybe_unused]]auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("E", regs.read("C"));
				return 1;
			}
		},
		{"LD E, D", 0x5a, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("E", regs.read("D"));
				return 1;
			}
		},
		{"LD E, E", 0x5b, 1,
			[]([[maybe_unused]] auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				return 1;
			}
		},
		{"LD E, H", 0x5c, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("E", regs.read("H"));
				return 1;
			}
		},
		{"LD E, L", 0x5d, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("E", regs.read("L"));
				return 1;
			}
		},
		{"LD E, (HL)", 0x5e, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				regs.write("E", memory.read(address));
				return 2;
			}
		},
		{"LD E, A", 0x5f, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("E", regs.read("A"));
				return 1;
			}
		},
		{"LD H, B", 0x60, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("H", regs.read("B"));
				return 1;
			}
		},
		{"LD H, C", 0x61, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("H", regs.read("C"));
				return 1;
			}
		},
		{"LD H, D", 0x62, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("H", regs.read("D"));
				return 1;
			}
		},
		{"LD H, E", 0x63, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("H", regs.read("E"));
				return 1;
			}
		},
		{"LD H, H", 0x64, 1,
			[]([[maybe_unused]] auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				return 1;
			}
		},
		{"LD H, L", 0x65, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("H", regs.read("L"));
				return 1;
			}
		},
		{"LD H, (HL)", 0x66, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				regs.write("H", memory.read(address));
				return 2;
			}
		},
		{"LD H, A", 0x67, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("H", regs.read("A"));
				return 1;
			}
		},
		{"LD L, B", 0x68, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("L", regs.read("B"));
				return 1;
			}
		},
		{"LD L, C", 0x69, 1,
			[]([[maybe_unused]]auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("L", regs.read("C"));
				return 1;
			}
		},
		{"LD L, D", 0x6a, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("L", regs.read("D"));
				return 1;
			}
		},
		{"LD L, E", 0x6b, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("L", regs.read("E"));
				return 1;
			}
		},
		{"LD L, H", 0x6c, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("L", regs.read("H"));
				return 1;
			}
		},
		{"LD L, L", 0x6d, 1,
			[]([[maybe_unused]] auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				return 1;
			}
		},
		{"LD L, (HL)", 0x6e, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				regs.write("L", memory.read(address));
				return 2;
			}
		},
		{"LD L, A", 0x6f, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("L", regs.read("A"));
				return 1;
			}
		},
		{"LD (HL), B", 0x70, 1,
			[](auto& regs,  auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				const auto value = regs.read("B");
				memory.write(address, value);
				return 2;
			}
		},
		{"LD (HL), C", 0x71, 1,
			[](auto& regs,  auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				const auto value = regs.read("C");
				memory.write(address, value);
				return 2;
			}
		},
		{"LD (HL), D", 0x72, 1,
			[](auto& regs,  auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				const auto value = regs.read("D");
				memory.write(address, value);
				return 2;
			}
		},
		{"LD (HL), E", 0x73, 1,
			[](auto& regs,  auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				const auto value = regs.read("E");
				memory.write(address, value);
				return 2;
			}
		},
		{"LD (HL), H", 0x74, 1,
			[](auto& regs,  auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				const auto value = regs.read("H");
				memory.write(address, value);
				return 2;
			}
		},
		{"LD (HL), L", 0x75, 1,
			[](auto& regs,  auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				const auto value = regs.read("L");
				memory.write(address, value);
				return 2;
			}
		},
		{"LD (HL), A", 0x77, 1,
			[](auto& regs,  auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				const auto value = regs.read("A");
				memory.write(address, value);
				return 2;
			}
		},
		{"LD A, B", 0x78, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("A", regs.read("B"));
				return 1;
			}
		},
		{"LD A, C", 0x79, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("A", regs.read("C"));
				return 1;
			}
		},
		{"LD A, D", 0x7a, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("A", regs.read("D"));
				return 1;
			}
		},
		{"LD A, E", 0x7b, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("A", regs.read("E"));
				return 1;
			}
		},
		{"LD A, H", 0x7c, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("A", regs.read("H"));
				return 1;
			}
		},
		{"LD A, L", 0x7d, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("A", regs.read("L"));
				return 1;
			}
		},
		{"LD A, (HL)", 0x7e, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				const auto value = memory.read(address);
				regs.write("A", value);
				return 2;
			}
		},

		{"LD A, A", 0x7f, 1,
			[]([[maybe_unused]] auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				return 1;
			}
		},

		// Decrement 16bit
		{"DEC BC", 0x0b, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("BC", regs.read("BC") - 1);
				return 2;
			}
		},
		{"DEC DE", 0x1b, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("DE", regs.read("DE") - 1);
				return 2;
			}
		},
		{"DEC HL", 0x2b, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("HL", regs.read("HL") - 1);
				return 2;
			}
		},
		{"DEC SP", 0x3b, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.write("SP", regs.read("SP") - 1);
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
		{"RRA", 0x1f, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				// TODO: Put into method
				const auto A = regs.read("A");
				const auto lsb = (A & (1 << 0)) >> 0;
				const auto A_new = static_cast<uint8_t>((A >> 1) + (regs.read_flag("C") << 7)); // Setting C here as bit 0, it's only difference from RLCA which uses msb
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
		{"CPL", 0x2f, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				const auto A = regs.read("A");
				const auto A_new = A ^ 0xff;
				regs.write("A", A_new);
				regs.set_flag("N", true);
				regs.set_flag("H", true);
				return 1;
			}
		},
		{"CCF", 0x3f, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				regs.set_flag("N", false);
				regs.set_flag("H", false);
				regs.set_flag("C", !regs.read_flag("C"));
				return 1;
			}
		},
		// -------------------- End of ungrouped instructions ----------------------

		// Add 16bit
		// TODO: Put these adds into method
		{"ADD HL, BC", 0x09, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_add("HL", "BC", regs);
				return 2;
			}
		},
		{"ADD HL, DE", 0x19, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_add("HL", "DE", regs);
				return 2;
			}
		},
		{"ADD HL, HL", 0x29, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_add("HL", "HL", regs);
				return 2;
			}
		},
		{"ADD HL, SP", 0x39, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_add("HL", "SP", regs);
				return 2;
			}
		},

		// Add 8bit
		{"ADD A, B", 0x80, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_add("A", "B", regs);
				return 1;
			}
		},
		{"ADD A, C", 0x81, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_add("A", "C", regs);
				return 1;
			}
		},
		{"ADD A, D", 0x82, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_add("A", "D", regs);
				return 1;
			}
		},
		{"ADD A, E", 0x83, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_add("A", "E", regs);
				return 1;
			}
		},
		{"ADD A, H", 0x84, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_add("A", "H", regs);
				return 1;
			}
		},
		{"ADD A, L", 0x85, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_add("A", "L", regs);
				return 1;
			}
		},
		{"ADD A, (HL)", 0x86, 1,
			[](auto& regs,  auto& memory, [[maybe_unused]] const auto& PC) {
				// TODO: Put into function
				const auto address = regs.read("HL");
				const auto value = memory.read(address);
				const auto A_old = regs.read("A");
				const auto A_new = static_cast<uint8_t>(A_old + value);

				regs.write("A", A_new);
				regs.set_flag("Z", A_new == 0);
				regs.set_flag("N", false);
				regs.set_flag("H", half_carry_add_8bit(A_old, value));
				regs.set_flag("C", carry_add_8bit(A_old, value));
				return 2;
			}
		},
		{"ADD A, L", 0x87, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_add("A", "A", regs);
				return 1;
			}
		},

		// 8bit add with carry
		{"ADC A, B", 0x88, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_addc("A", "B", regs);
				return 2;
			}
		},
		{"ADC A, C", 0x89, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_addc("A", "C", regs);
				return 2;
			}
		},
		{"ADC A, D", 0x8a, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_addc("A", "D", regs);
				return 2;
			}
		},
		{"ADC A, E", 0x8b, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_addc("A", "E", regs);
				return 2;
			}
		},
		{"ADC A, H", 0x8c, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_addc("A", "H", regs);
				return 2;
			}
		},
		{"ADC A, L", 0x8d, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_addc("A", "L", regs);
				return 2;
			}
		},
		{"ADC A, (HL)", 0x8e, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				const auto value = memory.read(address);
				instruction_addc("A", value, regs);
				return 2;
			}
		},
		{"ADC A, A", 0x8f, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_addc("A", "A", regs);
				return 2;
			}
		},

		// 8bit sub
		{"SUB B", 0x90, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sub("A", "B", regs);
				return 1;
			}
		},
		{"SUB C", 0x91, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sub("A", "C", regs);
				return 1;
			}
		},
		{"SUB D", 0x92, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sub("A", "D", regs);
				return 1;
			}
		},
		{"SUB E", 0x93, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sub("A", "E", regs);
				return 1;
			}
		},
		{"SUB H", 0x94, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sub("A", "H", regs);
				return 1;
			}
		},
		{"SUB L", 0x95, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sub("A", "L", regs);
				return 1;
			}
		},
		{"SUB (HL)", 0x96, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				const auto value = memory.read(address);
				instruction_sub("A", value, regs);
				return 1;
			}
		},
		{"SUB A", 0x97, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sub("A", "A", regs);
				return 1;
			}
		},


		// Jumps/calls
		{"JR s8", 0x18, 2,
			[](auto& regs,  auto& memory, const auto& PC) {
				const auto value = memory.read(PC + 1);
				const auto PC_new = PC + value - 2; // Instruction size is 2 so it must be subtracted here in advance for Instruction to work properly
				regs.write("PC", PC_new);
				return 3;
			}
		},
		{"JR, Z s8", 0x28, 2,
			[](auto& regs,  auto& memory, const auto& PC) {
				const auto value = memory.read(PC + 1);
				if (regs.read_flag("Z")) {
					const auto PC_new = PC + value - 2; // Instruction size is 2 so it must be subtracted here in advance for Instruction to work properly
					regs.write("PC", PC_new);
					return 3;
				}
				return 2;
			}
		},
		{"JR, C s8", 0x38, 2,
			[](auto& regs,  auto& memory, const auto& PC) {
				const auto value = memory.read(PC + 1);
				if (regs.read_flag("C")) {
					const auto PC_new = PC + value - 2; // Instruction size is 2 so it must be subtracted here in advance for Instruction to work properly
					regs.write("PC", PC_new);
					return 3;
				}
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
