#pragma once

#include <vector>
#include <iostream>
#include <cassert>

#include "registers.h"
#include "memory.h"

struct Instruction {
	std::string mnemonic;
	uint16_t opcode;
	uint8_t size;
	uint8_t cycles;
};

struct InstructionNew {
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


	// TODO: Merge (half) carries somehow?
	// TODO: Are (half) carries correct?
	// Detect half-carry for addition, see https://robdor.com/2016/08/10/gameboy-emulator-half-carry-flag/
	[[nodiscard]] auto half_carry_add_8bit(const uint16_t a, const uint16_t b) const {
		return (((a & 0xf) + (b & 0xf)) & 0x10) > 0;
	}
	[[nodiscard]] auto half_carry_add_16bit(const uint16_t a, const uint16_t b) const {
		return (((a & 0x0fff) + (b & 0x0fff)) & 0x1000) > 0;
	}

	[[nodiscard]] auto carry_add_8bit(const uint16_t a, const uint16_t b) const {
		return (((a & 0xff) + (b & 0xff)) & 0x100) > 0;
	}
	[[nodiscard]] auto carry_add_16bit(const uint16_t a, const uint16_t b) const {
		return (((a & 0xffff) + (b & 0xffff)) & 0x10000) > 0;
	}

	// https://www.reddit.com/r/EmuDev/comments/4clh23/trouble_with_halfcarrycarry_flag/
	[[nodiscard]] auto half_carry_sub_8bit(const uint16_t a, const uint16_t b) const {
		return ((a & 0x0f) - (b & 0x0f)) < 0;
	}

	[[nodiscard]] auto half_carry_sub_16bit(const uint16_t a, const uint16_t b) const {
		return ((a & 0x0fff) - (b & 0xfff)) < 0;
	}

	[[nodiscard]] auto carry_sub_8bit(const uint16_t a, const uint16_t b) const {
		return ((a & 0xff) - (b & 0xff)) < 0;
	}
	[[nodiscard]] auto carry_sub_16bit(const uint16_t a, const uint16_t b) const {
		return ((a & 0xffff) - (b & 0xffff)) < 0;
	}

	[[nodiscard]] auto execute_next() {
		const auto instruction_start = regs_.read("PC");
		const auto opcode = memory_.read(instruction_start);

		if (opcode == 0xBC) {
			throw std::runtime_error("16-bit opcodes not implemented yet.");
		}

		const auto instructionNew = find_by_opcodeNew(opcode);
		// TODO: Remove this if and move this check into find_by_opcode
		if (instructionNew != end(instructionsNew_)) {
			auto cycles = (*instructionNew)(regs_, memory_, regs_.read("PC"));
			return cycles;
		}

		const auto instruction = find_by_opcode(opcode);

		switch(opcode) {

			// Increment
			case 0x04: // INC B
				instruction_inc("B");
				break;
			case 0x14: // INC D
				instruction_inc("D");
				break;
			case 0x24: // INC H
				instruction_inc("H");
				break;
			case 0x34: // INC (HL)
				// TODO: Use instruction_inc method?
				{
					const auto address = regs_.read("HL");
					const auto old_value = memory_.read(address);
					const auto new_value = static_cast<uint8_t>(old_value + 1);
					memory_.write(address, new_value);

					regs_.set_flag("Z", new_value == 0x00);
					regs_.set_flag("N", false);
					regs_.set_flag("H", half_carry_add_8bit(old_value, 1));
				}
				break;

			case 0x05: // DEC B
				instruction_dec("B");
				break;
			case 0x06: // LD B, d8
				regs_.write("B", memory_.read(instruction_start + 1));
				break;
			case 0x07: // RLCA
				{
					// TODO: Put into method
					const auto A = regs_.read("A");
					const auto msb = (A & (1 << 7)) >> 7;
					const auto A_new = static_cast<uint8_t>((A << 1) + msb);
					regs_.write("A", A_new);

					regs_.set_flag("Z", false);
					regs_.set_flag("N", false);
					regs_.set_flag("H", false);
					regs_.set_flag("C", static_cast<bool>(msb));
				}
				break;
			case 0x08: // LD (a16), SP
				{
					// TODO: Put into method
					const auto address = static_cast<uint16_t>((memory_.read(instruction_start + 1) << 8) + memory_.read(instruction_start + 2));
					const auto SP = regs_.read("SP");
					memory_.write(address, static_cast<uint8_t>(SP & 0x00ff));
					memory_.write(address + 1, static_cast<uint8_t>((SP & 0xff00) >> 8));
				}
				break;
			case 0x09: // ADD HL, BC
				{
					// TODO: Put into method
					const auto BC = regs_.read("BC");
					const auto HL_old = regs_.read("HL");
					const auto HL_new = static_cast<uint16_t>(HL_old + BC);

					regs_.write("HL", HL_new);

					regs_.set_flag("N", 0);
					regs_.set_flag("H", half_carry_add_16bit(HL_old, BC));
					regs_.set_flag("C", carry_add_16bit(HL_old, BC));
				}
				break;
			case 0xa: // LD A, (BC)
				{
					const auto address = regs_.read("BC");
					regs_.write("A", memory_.read(address));
				}
				break;
			case 0x0b: // DEC BC
				regs_.write("BC", regs_.read("BC") - 1);
				break;
			case 0x0c: // INC C
				instruction_inc("C");
				break;
			case 0x0d: // DEC C
				instruction_dec("C");
				break;
			case 0x0e: // LD C, d8
				{
					const auto value = memory_.read(instruction_start + 1);
					regs_.write("C", value);
				}
				break;
			case 0x0f: // RRCA
				{
					// TODO: Put into method
					const auto A = regs_.read("A");
					const auto lsb = (A & (1 << 0)) >> 0;
					const auto A_new = static_cast<uint8_t>((A >> 1) + (lsb << 7));
					regs_.write("A", A_new);

					regs_.set_flag("Z", false);
					regs_.set_flag("N", false);
					regs_.set_flag("H", false);
					regs_.set_flag("C", static_cast<bool>(lsb));
				}
				break;
			default:
				// TODO: Use hex instead of dec
				throw std::runtime_error("Opcode " + std::to_string(opcode) + "(dec) not implemented yet.");
				break;
		}

		regs_.write("PC", regs_.read("PC") + instruction.size);
		return instruction.cycles;
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
		{"LD (BC), A", 0x02, 1, 2},
		{"INC BC", 0x03, 1, 2},
		{"INC B", 0x04, 1, 1},
		{"DEC B", 0x05, 1, 1},
		{"LD B, d8", 0x06, 2, 2},
		{"RLCA", 0x07, 1, 1},
		{"LD (a16), SP", 0x08, 3, 5},
		{"ADD HL, BC", 0x09, 1, 2},
		{"LD A, (BC)", 0x0a, 1, 2},
		{"DEC BC", 0x0b, 1, 2},
		{"INC C", 0x0c, 1, 1},
		{"DEC C", 0x0d, 1, 1},
		{"LD C, d8", 0x0e, 2, 2},
		{"RRCA", 0x0f, 1, 1},
		// TODO: {"STOP", 0x10, 2, 1},
		{"LD DE, d16", 0x11, 3, 3},
		{"LD (DE), A", 0x12, 1, 2},
		{"INC DE", 0x13, 1, 2},
		{"INC D", 0x14, 1, 1},

		// TODO: JR NZ, s8 - 0x20
		{"LD HL, d16", 0x21, 3, 3},
		{"LD (HL+), A", 0x22, 1, 2},
		{"INC HL", 0x23, 1, 2},
		{"INC H", 0x24, 1, 1},

		// TODO: JR NC, s8 - 0x30
		{"LD SP, d16", 0x31, 3, 3},
		{"LD (HL-), A", 0x32, 1, 2},
		{"INC SP", 0x33, 1, 2},
		{"INC (HL)", 0x34, 1, 3},



	};

	std::vector<InstructionNew> instructionsNew_ = {
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


	};

	[[nodiscard]] const Instruction& find_by_opcode(const uint16_t opcode) {
		auto res = std::find_if(begin(instructions_), end(instructions_), [opcode](const auto& instruction) { return instruction.opcode ==  opcode; });
		if (res == end(instructions_)) {
			throw std::runtime_error("Opcode " + std::to_string(opcode) + " (dec) not found");
		}
		return *res;
	}

	// TODO: Return const instruction& not iterator
	[[nodiscard]] const decltype(instructionsNew_)::iterator find_by_opcodeNew(const uint16_t opcode) {
		auto res = std::find_if(begin(instructionsNew_), end(instructionsNew_), [opcode](const auto& instruction) { return instruction.opcode ==  opcode; });
		// TODO: Return back when new instructions class is fully used and the old one is removed
		// if (res == end(instructionsNew_)) {
			// throw std::runtime_error("Opcode " + std::to_string(opcode) + " (dec) not found");
		// }
		// TD<decltype(res)> td;
		return res;
	}

	template<size_t kSize>
	void instruction_inc(const char(&reg_name)[kSize]) {
		constexpr auto reg_name_size = kSize - 1; // Subtract \0 at the end
		// 8bit
		if constexpr (reg_name_size == 1) {
			const auto old_value = regs_.read(reg_name);
			const auto new_value = static_cast<uint8_t>(old_value + 1);
			regs_.write(reg_name, new_value);

			regs_.set_flag("Z", new_value == 0x00);
			regs_.set_flag("N", false);
			regs_.set_flag("H", half_carry_add_8bit(old_value, 1));
		}
		// 16bit
		else {
			assert(false && "Not implemented yet");
		}
	}

	template<size_t kSize>
	void instruction_dec(const char(&reg_name)[kSize]) {
		constexpr auto reg_name_size = kSize - 1; // Subtract \0 at the end
		// 8bit
		if constexpr (reg_name_size == 1) {
			const auto old_value = regs_.read(reg_name);
			const auto new_value = static_cast<uint8_t>(old_value - 1);
			regs_.write(reg_name, new_value);

			regs_.set_flag("Z", new_value == 0x00);
			regs_.set_flag("N", true);
			regs_.set_flag("H", half_carry_sub_8bit(old_value, 1));
		}
		// 16bit
		else {
			assert(false && "Not implemented yet");
		}
	}


};
