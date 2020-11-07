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

struct DisassemblyInfo {
	uint16_t address;
	uint16_t next_address;
	Instruction instruction;
	std::vector<uint8_t> memory_representation;
};

[[nodiscard]] const Instruction& find_by_opcode(const uint16_t opcode, const std::vector<Instruction>& instructions) {
	auto res = std::find_if(begin(instructions), end(instructions), [opcode](const auto& instruction) { return instruction.opcode ==  opcode; });
	if (res == end(instructions)) {
		auto stream = std::stringstream{};
		stream << std::hex << opcode;
		throw std::runtime_error("Opcode 0x" + stream.str() + " not found.");
	}
	return *res;
}

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

// TOOD: Cleanup these instruction_* functions

void instruction_rst(const uint8_t& value, Registers& regs, Memory& memory, const uint16_t& PC) {
	const auto PC_high = static_cast<uint8_t>((PC & 0xff00) >> 8);
	const auto PC_low = static_cast<uint8_t>(PC & 0x00ff);
	const auto SP = regs.read("SP");

	memory.write(SP - 1, PC_high);
	memory.write(SP - 2, PC_low);

	regs.write("SP", SP - 2);
	regs.write("PC", value - 1);
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

// TODO: Unit unit test for this
template<size_t kDestSize, typename ValueType>
void instruction_subc(const char(&dest_name)[kDestSize], const ValueType& value, Registers& regs) {
	constexpr auto real_size = kDestSize - 1;
	static_assert(real_size == 1, "Only 8bit add with carry supported.");
	static_assert(std::is_same_v<ValueType, uint8_t>, "Only 8bit add with carry supported.");

	const auto C = regs.read_flag("C");
	const auto dest_old = regs.read(dest_name);

	const auto dest_old_minus_carry = static_cast<uint8_t>(dest_old - C);
	auto half_carry = half_carry_sub_8bit(dest_old, C);
	auto carry = carry_sub_8bit(dest_old, C);

	const auto dest_new = static_cast<uint8_t>(dest_old_minus_carry  - value);
	half_carry |= half_carry_sub_8bit(dest_old_minus_carry, value);
	carry |= carry_sub_8bit(dest_old_minus_carry, value);

	regs.write(dest_name, dest_new);
	regs.set_flag("Z", dest_new == 0);
	regs.set_flag("N", true);
	regs.set_flag("H", half_carry);
	regs.set_flag("C", carry);
}

// TODO: Unit unit test for this
template<size_t kDestSize, size_t kSecondRegNameSize>
void instruction_subc(const char(&dest_name)[kDestSize], const char(&second_reg_name)[kSecondRegNameSize], Registers& regs) {
	instruction_subc(dest_name, regs.read(second_reg_name), regs);
}


template<size_t kDestSize, typename ValueType>
void instruction_and(const char(&dest_name)[kDestSize], const ValueType& value, Registers& regs) {
	constexpr auto real_size = kDestSize - 1;
	static_assert(real_size == 1, "Only 8bit AND supported.");
	static_assert(std::is_same_v<ValueType, uint8_t>, "Only 8bit values supported.");

	const auto dest_old = regs.read(dest_name);
	const auto dest_new = static_cast<uint8_t>(dest_old & value);

	regs.write(dest_name, dest_new);
	regs.set_flag("Z", dest_new == 0);
	regs.set_flag("N", false);
	regs.set_flag("H", true);
	regs.set_flag("C", false);

}
template<size_t kDestSize, size_t kSecondRegNameSize>
void instruction_and(const char(&dest_name)[kDestSize], const char(&second_reg_name)[kSecondRegNameSize], Registers& regs) {
	static_assert(kDestSize == kSecondRegNameSize, "Registers must be of a same size. And only 8bit are supported.");
	instruction_and(dest_name, regs.read(second_reg_name), regs);
}

template<size_t kDestSize, typename ValueType>
void instruction_xor(const char(&dest_name)[kDestSize], const ValueType& value, Registers& regs) {
	constexpr auto real_size = kDestSize - 1;
	static_assert(real_size == 1, "Only 8bit XOR supported.");
	static_assert(std::is_same_v<ValueType, uint8_t>, "Only 8bit values supported.");

	const auto dest_old = regs.read(dest_name);
	const auto dest_new = static_cast<uint8_t>(dest_old ^ value);

	regs.write(dest_name, dest_new);
	regs.set_flag("Z", dest_new == 0);
	regs.set_flag("N", false);
	regs.set_flag("H", false);
	regs.set_flag("C", false);

}
template<size_t kDestSize, size_t kSecondRegNameSize>
void instruction_xor(const char(&dest_name)[kDestSize], const char(&second_reg_name)[kSecondRegNameSize], Registers& regs) {
	static_assert(kDestSize == kSecondRegNameSize, "Registers must be of a same size. And only 8bit are supported.");
	instruction_xor(dest_name, regs.read(second_reg_name), regs);
}

template<size_t kDestSize, typename ValueType>
void instruction_or(const char(&dest_name)[kDestSize], const ValueType& value, Registers& regs) {
	constexpr auto real_size = kDestSize - 1;
	static_assert(real_size == 1, "Only 8bit XOR supported.");
	static_assert(std::is_same_v<ValueType, uint8_t>, "Only 8bit values supported.");

	const auto dest_old = regs.read(dest_name);
	const auto dest_new = static_cast<uint8_t>(dest_old | value);

	regs.write(dest_name, dest_new);
	regs.set_flag("Z", dest_new == 0);
	regs.set_flag("N", false);
	regs.set_flag("H", false);
	regs.set_flag("C", false);

}
template<size_t kDestSize, size_t kSecondRegNameSize>
void instruction_or(const char(&dest_name)[kDestSize], const char(&second_reg_name)[kSecondRegNameSize], Registers& regs) {
	static_assert(kDestSize == kSecondRegNameSize, "Registers must be of a same size. And only 8bit are supported.");
	instruction_or(dest_name, regs.read(second_reg_name), regs);
}

template<size_t kDestSize, typename ValueType>
void instruction_cp(const char(&dest_name)[kDestSize], const ValueType& value, Registers& regs) {
	constexpr auto real_size = kDestSize - 1;
	static_assert(real_size == 1, "Only 8bit XOR supported.");
	static_assert(std::is_same_v<ValueType, uint8_t>, "Only 8bit values supported.");

	const auto dest_old = regs.read(dest_name);
	const auto dest_new = static_cast<uint8_t>(dest_old - value);

	// regs.write(dest_name, dest_new);
	regs.set_flag("Z", dest_new == 0);
	regs.set_flag("N", true);
	regs.set_flag("H", half_carry_sub_8bit(dest_old, value));
	regs.set_flag("C", carry_sub_8bit(dest_old, value));

}
template<size_t kDestSize, size_t kSecondRegNameSize>
void instruction_cp(const char(&dest_name)[kDestSize], const char(&second_reg_name)[kSecondRegNameSize], Registers& regs) {
	static_assert(kDestSize == kSecondRegNameSize, "Registers must be of a same size. And only 8bit are supported.");
	instruction_cp(dest_name, regs.read(second_reg_name), regs);
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

		if (opcode == 0xCB) {
			throw std::runtime_error("16-bit opcodes not implemented yet.");
		}

		const auto instruction = find_by_opcode(opcode);
		const auto cycles = instruction(regs_, memory_, PC);
		return cycles;
	}

	[[nodiscard]] auto disassemble_next(const uint16_t& addr) const {
		auto regs = regs_;
		auto memory = memory_;
		const auto PC = addr;
		regs.write("PC", PC);
		const auto opcode = memory.read(PC);

		const auto instruction = find_by_opcode(opcode);

		[[maybe_unused]] const auto cycles = instruction(regs, memory, PC);
		auto memory_representation = std::vector<uint8_t>{};
		for (auto i = addr; i < addr + instruction.size; ++i) {
			memory_representation.push_back(memory.read(i));
		}

		const auto result = DisassemblyInfo{addr, regs.read("PC"), instruction, memory_representation};
		return result;
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
		// TODO: DI - 0xf3
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

		// Load with implicit 0xff prefix
		{"LD (a8), A", 0xe0, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto address = static_cast<uint16_t>(0xff00 + memory.read(PC + 1));
				const auto value = regs.read("A");
				memory.write(address, value);
				return 3;
			}
		},
		{"LD A, (a8)", 0xf0, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto address = static_cast<uint16_t>(0xff00 + memory.read(PC + 1));
				const auto value = memory.read(address);
				regs.write("A", value);
				return 3;
			}
		},
		{"LD (C), A", 0xe2, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = static_cast<uint16_t>(0xff00 + regs.read("C"));
				const auto value = regs.read("A");
				memory.write(address, value);
				return 2;
			}
		},
		{"LD A, (C)", 0xf2, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = static_cast<uint16_t>(0xff00 + regs.read("C"));
				const auto value = memory.read(address);
				regs.write("A", value);
				return 2;
			}
		},
		
		// POP
		{"POP BC", 0xc1, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto SP = regs.read("SP");
				const auto BC_new = static_cast<uint16_t>((memory.read(SP + 1) << 8) + memory.read(SP));
				regs.write("BC", BC_new);
				regs.write("SP", SP + 2);
				return 3;
			}
		},
		{"POP DE", 0xd1, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto SP = regs.read("SP");
				const auto DE_new = static_cast<uint16_t>((memory.read(SP + 1) << 8) + memory.read(SP));
				regs.write("DE", DE_new);
				regs.write("SP", SP + 2);
				return 3;
			}
		},
		{"POP HL", 0xe1, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto SP = regs.read("SP");
				const auto HL_new = static_cast<uint16_t>((memory.read(SP + 1) << 8) + memory.read(SP));
				regs.write("HL", HL_new);
				regs.write("SP", SP + 2);
				return 3;
			}
		},
		{"POP AF", 0xf1, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto SP = regs.read("SP");
				const auto AF_new = static_cast<uint16_t>((memory.read(SP + 1) << 8) + memory.read(SP));
				regs.write("AF", AF_new);
				regs.write("SP", SP + 2);
				return 3;
			}
		},

		// PUSH
		{"PUSH BC", 0xc5, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto SP = regs.read("SP");
				memory.write(SP - 1, regs.read("B"));
				memory.write(SP - 2, regs.read("C"));
				regs.write("SP", SP - 2);
				return 4;
			}
		},
		{"PUSH DE", 0xd5, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto SP = regs.read("SP");
				memory.write(SP - 1, regs.read("D"));
				memory.write(SP - 2, regs.read("E"));
				regs.write("SP", SP - 2);
				return 4;
			}
		},
		{"PUSH HL", 0xe5, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto SP = regs.read("SP");
				memory.write(SP - 1, regs.read("H"));
				memory.write(SP - 2, regs.read("L"));
				regs.write("SP", SP - 2);
				return 4;
			}
		},
		{"PUSH AF", 0xf5, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto SP = regs.read("SP");
				memory.write(SP - 1, regs.read("A"));
				memory.write(SP - 2, regs.read("F"));
				regs.write("SP", SP - 2);
				return 4;
			}
		},

		// LD with signed add
		// TODO: Check if subtraction works correctly
		{"LD HL, SP+s8", 0xf8, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto value = static_cast<int8_t>(memory.read(PC + 1));
				const auto SP = regs.read("SP");
				const auto HL_old = regs.read("HL");

				const auto intermediate_result = static_cast<uint16_t>(SP + value);
				auto half_carry = half_carry_add_16bit(SP, value);
				auto carry = carry_add_16bit(SP, value);

				const auto HL_new = static_cast<uint16_t>(HL_old + intermediate_result);
				half_carry |= half_carry_add_16bit(HL_old, intermediate_result);
				carry |= carry_add_16bit(HL_old, intermediate_result);

				regs.write("HL", HL_new);
				regs.set_flag("Z", false);
				regs.set_flag("N", false);
				regs.set_flag("H", half_carry);
				regs.set_flag("C", carry);
				return 3;
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
		{"SBC A, B", 0x98, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_subc("A", "B", regs);
				return 1;
			}
		},
		{"SBC A, C", 0x99, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_subc("A", "C", regs);
				return 1;
			}
		},
		{"SBC A, D", 0x9a, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_subc("A", "D", regs);
				return 1;
			}
		},
		{"SBC A, E", 0x9b, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_subc("A", "E", regs);
				return 1;
			}
		},
		{"SBC A, H", 0x9c, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_subc("A", "H", regs);
				return 1;
			}
		},
		{"SBC A, L", 0x9d, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_subc("A", "L", regs);
				return 1;
			}
		},
		{"SBC A, (HL)", 0x9e, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				const auto value = memory.read(address);
				instruction_subc("A", value, regs);
				return 2;
			}
		},
		{"SBC A, A", 0x9f, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_subc("A", "A", regs);
				return 1;
			}
		},

		// AND
		{"AND B", 0xa0, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_and("A", "B", regs);
				return 1;
			}
		},
		{"AND C", 0xa1, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_and("A", "C", regs);
				return 1;
			}
		},
		{"AND D", 0xa2, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_and("A", "D", regs);
				return 1;
			}
		},
		{"AND E", 0xa3, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_and("A", "E", regs);
				return 1;
			}
		},
		{"AND H", 0xa4, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_and("A", "H", regs);
				return 1;
			}
		},
		{"AND L", 0xa5, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_and("A", "L", regs);
				return 1;
			}
		},
		{"AND (HL)", 0xa6, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				const auto value = memory.read(address);
				instruction_and("A", value, regs);
				return 1;
			}
		},
		{"AND A", 0xa7, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_and("A", "A", regs);
				return 1;
			}
		},

		// XOR
		{"XOR B", 0xa8, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_xor("A", "B", regs);
				return 1;
			}
		},
		{"XOR C", 0xa9, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_xor("A", "C", regs);
				return 1;
			}
		},
		{"XOR D", 0xaa, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_xor("A", "D", regs);
				return 1;
			}
		},
		{"XOR E", 0xab, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_xor("A", "E", regs);
				return 1;
			}
		},
		{"XOR H", 0xac, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_xor("A", "H", regs);
				return 1;
			}
		},
		{"XOR L", 0xad, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_xor("A", "L", regs);
				return 1;
			}
		},
		{"XOR (HL)", 0xae, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				const auto value = memory.read(address);
				instruction_xor("A", value, regs);
				return 1;
			}
		},
		{"XOR A", 0xaf, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_xor("A", "A", regs);
				return 1;
			}
		},

		// OR
		{"OR B", 0xb0, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_or("A", "B", regs);
				return 1;
			}
		},
		{"OR C", 0xb1, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_or("A", "C", regs);
				return 1;
			}
		},
		{"OR D", 0xb2, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_or("A", "D", regs);
				return 1;
			}
		},
		{"OR E", 0xb3, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_or("A", "E", regs);
				return 1;
			}
		},
		{"OR H", 0xb4, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_or("A", "H", regs);
				return 1;
			}
		},
		{"OR L", 0xb5, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_or("A", "L", regs);
				return 1;
			}
		},
		{"OR (HL)", 0xb6, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				const auto value = memory.read(address);
				instruction_or("A", value, regs);
				return 1;
			}
		},
		{"OR A", 0xb7, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_or("A", "A", regs);
				return 1;
			}
		},

		// CP
		{"CP B", 0xb8, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_cp("A", "B", regs);
				return 1;
			}
		},
		{"CP C", 0xb9, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_cp("A", "C", regs);
				return 1;
			}
		},
		{"CP D", 0xba, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_cp("A", "D", regs);
				return 1;
			}
		},
		{"CP E", 0xbb, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_cp("A", "E", regs);
				return 1;
			}
		},
		{"CP H", 0xbc, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_cp("A", "H", regs);
				return 1;
			}
		},
		{"CP L", 0xbd, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_cp("A", "L", regs);
				return 1;
			}
		},
		{"CP (HL)", 0xbe, 1,
			[](auto& regs, auto& memory, [[maybe_unused]] const auto& PC) {
				const auto address = regs.read("HL");
				const auto value = memory.read(address);
				instruction_cp("A", value, regs);
				return 1;
			}
		},
		{"CP A", 0xbf, 1,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_cp("A", "A", regs);
				return 1;
			}
		},

		// Operations on A register
		{"ADD A, d8", 0xc6, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto value = memory.read(PC + 1);
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
		{"SUB A, d8", 0xd6, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto value = memory.read(PC + 1);
				const auto A_old = regs.read("A");
				const auto A_new = static_cast<uint8_t>(A_old - value);

				regs.write("A", A_new);
				regs.set_flag("Z", A_new == 0);
				regs.set_flag("N", true);
				regs.set_flag("H", half_carry_sub_8bit(A_old, value));
				regs.set_flag("C", carry_sub_8bit(A_old, value));
				return 2;
			}
		},
		{"AND d8", 0xe6, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto value = memory.read(PC + 1);
				instruction_and("A", value, regs);
				return 2;
			}
		},
		{"OR d8", 0xf6, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto value = memory.read(PC + 1);
				instruction_or("A", value, regs);
				return 2;
			}
		},

		// Signed add for SP
		// TODO: Check if subtraction works correctly
		{"ADD SP, s8", 0xe8, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto value = static_cast<int8_t>(memory.read(PC + 1));
				const auto SP_old = regs.read("SP");
				const auto SP_new = static_cast<uint16_t>(SP_old + value);

				regs.write("SP", SP_new);
				regs.set_flag("Z", false);
				regs.set_flag("N", false);
				regs.set_flag("H", half_carry_add_16bit(SP_old, value));
				regs.set_flag("C", carry_add_16bit(SP_old, value));
				return 4;
			}
		},

		// Jumps/calls
		// TODO: https://forums.emulator-zone.com/showthread.php?t=25471 mentions that PC points to the next instruction before
		//       current instruction is executed, do it that way?
		// NOTE: Because the PC is incremented by instruction size in Instruction::operator() and these jumps/calls manipulate PC,
		//       These instructions have to subtract their instruction size from PC to compensate for that.
		//       In some places, "- 2 + 2" can be seen, minus 2 is to remove instruction size that will be automatically added (works fine for most instructions),
		//       plus 2 is there because relative jumps works with PC pointing to the next instruction but I'm incrementing PC after execution, see TODO above
		{"JR s8", 0x18, 2,
			[](auto& regs,  auto& memory, const auto& PC) {
				const auto value = static_cast<int8_t>(memory.read(PC + 1));
				const auto PC_new = static_cast<uint16_t>(PC + value);
				// const auto PC_new = PC + value - 2 + 2; // Instruction size is 2 so it must be subtracted here in advance for Instruction to work properly
				regs.write("PC", PC_new - 2 + 2);
				return 3;
			}
		},
		{"JR, Z s8", 0x28, 2,
			[](auto& regs,  auto& memory, const auto& PC) {
				if (regs.read_flag("Z")) {
					const auto value = static_cast<int8_t>(memory.read(PC + 1));
					const auto PC_new = static_cast<uint16_t>(PC + value);
					regs.write("PC", PC_new - 2 + 2);
					return 3;
				}
				return 2;
			}
		},
		{"JR, C s8", 0x38, 2,
			[](auto& regs,  auto& memory, const auto& PC) {
				if (regs.read_flag("C")) {
					const auto value = static_cast<int8_t>(memory.read(PC + 1));
					const auto PC_new = static_cast<uint16_t>(PC + value);
					regs.write("PC", PC_new - 2 + 2);
					return 3;
				}
				return 2;
			}
		},
		{"RET NZ", 0xc0, 1,
			[](auto& regs,  auto& memory, [[maybe_unused]] const auto& PC) {
				if (!regs.read_flag("Z")) {
					const auto SP = regs.read("SP");
					const auto PC_new = static_cast<uint16_t>((memory.read(SP + 1) << 8) + memory.read(SP));
					regs.write("PC", PC_new - 1);
					regs.write("SP", SP + 2);
					return 5;
				}
				return 2;
			}
		},
		{"RET NC", 0xd0, 1,
			[](auto& regs,  auto& memory, [[maybe_unused]] const auto& PC) {
				if (!regs.read_flag("C")) {
					const auto SP = regs.read("SP");
					const auto PC_new = static_cast<uint16_t>((memory.read(SP + 1) << 8) + memory.read(SP));
					regs.write("PC", PC_new - 1);
					regs.write("SP", SP + 2);
					return 5;
				}
				return 2;
			}
		},
		{"RET Z", 0xc8, 1,
			[](auto& regs,  auto& memory, [[maybe_unused]] const auto& PC) {
				if (regs.read_flag("Z")) {
					const auto SP = regs.read("SP");
					const auto PC_new = static_cast<uint16_t>((memory.read(SP + 1) << 8) + memory.read(SP));
					regs.write("PC", PC_new - 1);
					regs.write("SP", SP + 2);
					return 5;
				}
				return 2;
			}
		},
		{"RET C", 0xd8, 1,
			[](auto& regs,  auto& memory, [[maybe_unused]] const auto& PC) {
				if (regs.read_flag("C")) {
					const auto SP = regs.read("SP");
					const auto PC_new = static_cast<uint16_t>((memory.read(SP + 1) << 8) + memory.read(SP));
					regs.write("PC", PC_new - 1);
					regs.write("SP", SP + 2);
					return 5;
				}
				return 2;
			}
		},
		{"JP NZ, a16", 0xc2, 3,
			[](auto& regs, auto& memory, const auto& PC) {
				if (regs.read_flag("Z") == false) {
					const auto PC_new = static_cast<uint16_t>((memory.read(PC + 2) << 8) + memory.read(PC + 1));
					regs.write("PC", PC_new - 3);
					return 4;
				}
				return 3;
			}
		},
		{"JP NC, a16", 0xd2, 3,
			[](auto& regs, auto& memory, const auto& PC) {
				if (regs.read_flag("C") == false) {
					const auto PC_new = static_cast<uint16_t>((memory.read(PC + 2) << 8) + memory.read(PC + 1));
					regs.write("PC", PC_new - 3);
					return 4;
				}
				return 3;
			}
		},
		{"JP a16", 0xc3, 3,
			[](auto& regs, auto& memory, const auto& PC) {
				// TODO: Put into method
				const auto PC_new = static_cast<uint16_t>((memory.read(PC + 2) << 8) + memory.read(PC + 1));
				regs.write("PC", PC_new - 3);
				return 4;
			}
		},
		{"CALL NZ, a16", 0xc4, 3,
			[](auto& regs, auto& memory, const auto& PC) {
				if (regs.read_flag("Z") == false) {
					// TODO: Put into method
					const auto return_address = PC + 3;
					const auto return_address_high = static_cast<uint8_t>((return_address & 0xff00) >> 8);
					const auto return_address_low = static_cast<uint8_t>(return_address & 0x00ff);

					const auto SP = regs.read("SP");
					memory.write(SP - 1, return_address_high);
					memory.write(SP - 2, return_address_low);
					regs.write("SP", SP - 2);

					const auto call_address = static_cast<uint16_t>((memory.read(PC + 2) << 8) + memory.read(PC + 1));

					regs.write("PC", call_address - 3);
					return 6;
				}
				return 3;
			}
		},
		{"CALL NC, a16", 0xd4, 3,
			[](auto& regs, auto& memory, const auto& PC) {
				if (regs.read_flag("C") == false) {
					// TODO: Put into method
					const auto return_address = PC + 3;
					const auto return_address_high = static_cast<uint8_t>((return_address & 0xff00) >> 8);
					const auto return_address_low = static_cast<uint8_t>(return_address & 0x00ff);

					const auto SP = regs.read("SP");
					memory.write(SP - 1, return_address_high);
					memory.write(SP - 2, return_address_low);
					regs.write("SP", SP - 2);

					const auto call_address = static_cast<uint16_t>((memory.read(PC + 2) << 8) + memory.read(PC + 1));

					regs.write("PC", call_address - 3);
					return 6;
				}
				return 3;
			}
		},
		{"RST 0", 0xc7, 1,
			[](auto& regs, auto& memory, const auto& PC) {
				instruction_rst(0x00, regs, memory, PC);
				return 4;
			}
		},
		{"RST 2", 0xd7, 1,
			[](auto& regs, auto& memory, const auto& PC) {
				instruction_rst(0x10, regs, memory, PC);
				return 4;
			}
		},
		{"RST 4", 0xe7, 1,
			[](auto& regs, auto& memory, const auto& PC) {
				instruction_rst(0x20, regs, memory, PC);
				return 4;
			}
		},
		{"RST 6", 0xf7, 1,
			[](auto& regs, auto& memory, const auto& PC) {
				instruction_rst(0x30, regs, memory, PC);
				return 4;
			}
		},
		{"RST 1", 0xcf, 1,
			[](auto& regs, auto& memory, const auto& PC) {
				instruction_rst(0x08, regs, memory, PC);
				return 4;
			}
		},
		{"RST 3", 0xdf, 1,
			[](auto& regs, auto& memory, const auto& PC) {
				instruction_rst(0x18, regs, memory, PC);
				return 4;
			}
		},
		{"RST 5", 0xef, 1,
			[](auto& regs, auto& memory, const auto& PC) {
				instruction_rst(0x28, regs, memory, PC);
				return 4;
			}
		},
		{"RST 7", 0xff, 1,
			[](auto& regs, auto& memory, const auto& PC) {
				instruction_rst(0x38, regs, memory, PC);
				return 4;
			}
		},
		{"JR NZ, s8", 0x20, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				if (!regs.read_flag("Z")) {
					const auto value = static_cast<int8_t>(memory.read(PC + 1));
					const auto PC_new = static_cast<uint16_t>(PC + value);
					regs.write("PC", PC_new - 2 + 2);
					return 3;
				}
				return 2;
			}
		},
		{"JR NC, s8", 0x30, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				if (!regs.read_flag("C")) {
					const auto value = static_cast<int8_t>(memory.read(PC + 1));
					const auto PC_new = static_cast<uint16_t>(PC + value);
					regs.write("PC", PC_new - 2 + 2);
					return 3;
				}
				return 2;
			}
		},


	};

	[[nodiscard]] const Instruction& find_by_opcode(const uint16_t opcode) const {
		return ::find_by_opcode(opcode, instructions_);
	}

};
