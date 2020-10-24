#pragma once

#include <vector>
#include <iostream>

#include "registers.h"
#include "memory.h"

struct Instruction {
	std::string mnemonic;
	uint16_t opcode;
	uint8_t size;
	uint8_t cycles;
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

		const auto instruction = find_by_opcode(opcode);

		switch(opcode) {
			case 0x00:
				break;
			case 0x01:
				regs_.write("B", memory_.read(instruction_start + 1));
				regs_.write("C", memory_.read(instruction_start + 2));
				break;
			case 0x02:
				memory_.write(regs_.read("BC"), regs_.read("A"));
				break;
			case 0x03:
				regs_.write("BC", regs_.read("BC") + 1);
				break;
			case 0x04:
				{
					const auto B_old = regs_.read("B");
					const auto B_new = static_cast<uint8_t>(B_old + 1);
					regs_.write("B", B_new);


					regs_.set_flag("Z", B_new == 0x00);
					regs_.set_flag("N", false);

					const auto H = half_carry_add_8bit(B_old, 1);
					regs_.set_flag("H", H);
				}
				break;
			case 0x05:
				{
					const auto B_old = regs_.read("B");
					const auto B_new = static_cast<uint8_t>(B_old - 1);
					regs_.write("B", B_new);

					// TODO: Move into a function
					// From https://stackoverflow.com/questions/57817729/half-carry-flag-emulating-the-gba
					// TODO: How does it work? Is it correct?
					const auto H = (B_old & 0x0F) - 1 < 0;;

					regs_.set_flag("Z", B_new == 0x00);
					regs_.set_flag("N", true);
					regs_.set_flag("H", H);
				}
				break;
			case 0x06:
				regs_.write("B", memory_.read(instruction_start + 1));
				break;

			case 0x07:
				{
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
			case 0x08:
				{
					const auto address = static_cast<uint16_t>((memory_.read(instruction_start + 1) << 8) + memory_.read(instruction_start + 2));
					const auto SP = regs_.read("SP");
					memory_.write(address, static_cast<uint8_t>(SP & 0x00ff));
					memory_.write(address + 1, static_cast<uint8_t>((SP & 0xff00) >> 8));
				}
				break;
			case 0x09:
				{
					const auto BC = regs_.read("BC");
					const auto HL_old = regs_.read("HL");
					const auto HL_new = static_cast<uint16_t>(HL_old + BC);

					regs_.write("HL", HL_new);

					regs_.set_flag("N", 0);
					regs_.set_flag("H", half_carry_add_16bit(HL_old, BC));
					regs_.set_flag("C", carry_add_16bit(HL_old, BC));
				}
				break;
			case 0xa:
				{
					const auto address = regs_.read("BC");
					regs_.write("A", memory_.read(address));
				}
				break;
			case 0x0b:
				regs_.write("BC", regs_.read("BC") - 1);
				break;
			case 0x0c:
				{
					const auto C_old = regs_.read("C");
					const auto C_new = static_cast<uint8_t>(C_old + 1);

					regs_.write("C", C_new);

					regs_.set_flag("Z", C_new == 0x00);
					regs_.set_flag("N", false);
					regs_.set_flag("H", half_carry_add_8bit(C_old, 1));
				}
				break;
			case 0x0d:
				{
					const auto C_old = regs_.read("C");
					const auto C_new = static_cast<uint8_t>(C_old - 1);

					regs_.write("C", C_new);

					regs_.set_flag("Z", C_new == 0x00);
					regs_.set_flag("N", true);
					regs_.set_flag("H", half_carry_sub_8bit(C_old, 1));
				}
				break;
			case 0x0e:
				{
					const auto value = memory_.read(instruction_start + 1);
					regs_.write("C", value);
				}
				break;
			case 0x0f:
				{
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
		{"NOP", 0x00, 1, 1},
		{"LD BC, d16", 0x01, 3, 3},
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
	};

	const Instruction& find_by_opcode(const uint16_t opcode) {
		auto res = std::find_if(begin(instructions_), end(instructions_), [opcode](const auto& instruction) { return instruction.opcode ==  opcode; });
		if (res == end(instructions_)) {
			throw std::runtime_error("Opcode " + std::to_string(opcode) + " (dec) not found");
		}
		return *res;
	}


};
