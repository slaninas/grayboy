#pragma once

#include <vector>
#include <iostream>

#include "registers.h"

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
	using MemoryType = std::array<uint8_t, 1 << 16>;

	// TODO: Make move possible?
	Cpu(const MemoryType& memory) :
		memory_{memory}
	{}


	void clear_registers() {
		regs_.clear();
	}

	[[nodiscard]] auto execute_next() {
		const auto instruction_start = regs_.read_PC();
		const auto opcode = memory_[instruction_start];

		if (opcode == 0xBC) {
			throw std::runtime_error("16-bit opcodes not implemented yet.");
		}

		const auto instruction = find_by_opcode(opcode);

		switch(opcode) {
			case 0x01:
				regs_.write_B(memory_[instruction_start + 1]);
				regs_.write_C(memory_[instruction_start + 2]);
				break;
			case 0x02:
				memory_[regs_.read_BC()] = regs_.read_A();
				break;
			default:
				throw std::runtime_error("Opcode not implemented yet.");
				break;
		}

		regs_.write_PC(regs_.read_PC() + instruction.size);
		return instruction.cycles;
	}

	[[nodiscard]] auto registers_dump() const {
		return regs_;
	}

	[[nodiscard]] auto memory_dump() const {
		return memory_;
	}

	[[nodiscard]] auto& registers() {
		return regs_;
	}


private:
	Registers regs_ = {};
	// uint8_t flags_;

	MemoryType memory_ = {};

	std::vector<Instruction> instructions_ = {
		{"LD BC, d16", 0x01, 3, 3},
		{"LD (BC), A", 0x02, 1, 2}
	};

	const Instruction& find_by_opcode(const uint16_t opcode) {
		auto res = std::find_if(begin(instructions_), end(instructions_), [opcode](const auto& instruction) { return instruction.opcode ==  opcode; });
		if (res == end(instructions_)) {
			throw std::runtime_error("Opcode not found");
		}
		return *res;
	}


};
