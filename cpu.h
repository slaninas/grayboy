#pragma once

#include <vector>
#include <iostream>
#include <sstream>
#include <cassert>
#include <type_traits>

#include "registers.h"
#include "memory.h"

#include "instructions.h"
#include "instruction_utils.h"


struct DisassemblyInfo {
	uint16_t address;
	uint16_t next_address;
	Instruction instruction;
	std::vector<uint8_t> memory_representation;
};

[[nodiscard]] auto find_by_opcode(const uint16_t opcode, const std::vector<Instruction>& instructions) -> Instruction;


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

	[[nodiscard]] auto get_opcode(const uint16_t& starting_address) const {
		const auto first_byte = static_cast<uint16_t>(memory_.read(starting_address));
		if (first_byte == 0xcb) {
			const auto second_byte = memory_.read(starting_address + 1);
			return static_cast<uint16_t>((first_byte << 8) + second_byte);
		}
		return first_byte;
	}


	[[nodiscard]] auto execute_next() {
		const auto PC = regs_.read("PC");
		const auto opcode = get_opcode(PC);

		const auto instruction = find_by_opcode(opcode);
		const auto cycles = instruction(regs_, memory_, PC);
		std::cout << "executing 0x" << instruction.mnemonic << '\n';
		// TODO: Unit test are not prepared for this, if getRandomMemory() set 0xff02 to 0x81, it may fail. Make getRandomMemory() to set 0xff02 not to 0x81?
		// if (memory_.read(0xff02) == 0x81) {
			// auto c = static_cast<char>(memory_.read(0xff01));
			// std::cout << c;
			// memory_.write(0xff02, 0x00);
		// }
		return cycles;
	}

	[[nodiscard]] auto disassemble_next(const uint16_t& starting_address) const {
		auto regs = regs_;
		auto memory = memory_;
		regs.write("PC", starting_address);
		const auto opcode = get_opcode(starting_address);

		const auto instruction = find_by_opcode(opcode);

		[[maybe_unused]] const auto cycles = instruction(regs, memory, starting_address);
		auto memory_representation = std::vector<uint8_t>{};
		for (auto address = starting_address; address < starting_address + instruction.size; ++address) {
			memory_representation.push_back(memory.read(address));
		}

		return DisassemblyInfo{starting_address, regs.read("PC"), instruction, memory_representation};
	}


	[[nodiscard]] auto registers_dump() const {
		return regs_;
	}

	[[nodiscard]] auto memory_dump() const {
		return memory_.dump();
	}

	[[nodiscard]] auto registers() -> auto& {
		return regs_;
	}


private:
	MemoryType memory_ = {};
	Registers regs_ = {};

	// See https://meganesulli.com/generate-gb-opcodes/
	std::vector<Instruction> instructions_ = get_all_instructions();

	[[nodiscard]] auto find_by_opcode(const uint16_t opcode) const -> Instruction {
		return ::find_by_opcode(opcode, instructions_);
	}

};
