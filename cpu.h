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

					// TODO: Move into a function
					const auto H = (B_old & 0x08) != 0 && (B_new & 0x08) == 0;

					regs_.set_flag("Z", B_new == 0x00);
					regs_.set_flag("N", false);
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
			default:
				throw std::runtime_error("Opcode not implemented yet.");
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

	std::vector<Instruction> instructions_ = {
		{"NOP", 0x00, 1, 1},
		{"LD BC, d16", 0x01, 3, 3},
		{"LD (BC), A", 0x02, 1, 2},
		{"INC BC", 0x03, 1, 2},
		{"INC B", 0x04, 1, 1},
		{"DEC B", 0x05, 1, 1},
	};

	const Instruction& find_by_opcode(const uint16_t opcode) {
		auto res = std::find_if(begin(instructions_), end(instructions_), [opcode](const auto& instruction) { return instruction.opcode ==  opcode; });
		if (res == end(instructions_)) {
			throw std::runtime_error("Opcode not found");
		}
		return *res;
	}


};
