#pragma once

#include "instruction_utils.h"
#include "instructions.h"
#include "memory.h"
#include "registers.h"

#include <vector>

struct DisassemblyInfo {
	uint16_t address;
	uint16_t next_address;
	Instruction instruction;
	std::vector<uint8_t> memory_representation;
};

class Cpu;
inline void log(const Cpu& cpu);

// Make exceptions asserts and run in debug
// TODO: Add unit tests
class Cpu {
public:
	using MemoryType = Memory;

	Cpu() = default;
	Cpu(const Registers& regs) : regs_{regs} {}


	[[nodiscard]] auto execute_next(Memory& memory) -> uint64_t
	{
		const auto PC = regs_.read("PC");
		const auto opcode = get_opcode(PC, memory);
		const auto instruction = find_by_opcode(opcode);

		const auto cycles = execute_opcode(instruction.opcode, PC, regs_, memory);
		regs_.write("PC", regs_.read("PC") + instruction.size);

		return cycles;
	}

	[[nodiscard]] auto execute_opcode(const uint16_t& opcode, const uint16_t& PC, Registers& regs, Memory& memory) const -> uint8_t;


	[[nodiscard]] auto disassemble_next(const uint16_t& starting_address, const Memory& memory) const -> DisassemblyInfo {
		auto regs = regs_;
		auto memory2 = memory;
		regs.write("PC", starting_address);
		const auto opcode = get_opcode(starting_address, memory2);

		const auto instruction = find_by_opcode(opcode);
		const auto PC = regs.read("PC");

		[[maybe_unused]] const auto cycles = execute_opcode(instruction.opcode, PC, regs, memory2);
		auto memory_representation = std::vector<uint8_t>{};
		for (auto address = starting_address; address < starting_address + instruction.size; ++address) {
			memory_representation.push_back(memory2.read(address));
		}

		return DisassemblyInfo{starting_address, regs.read("PC"), instruction, memory_representation};
	}

	[[nodiscard]] auto registers() -> auto&
	{
		return regs_;
	}

	[[nodiscard]] auto registers() const  -> const auto&
	{
		return regs_;
	}

private:

	[[nodiscard]] auto find_by_opcode(const uint16_t opcode) const -> Instruction
	{
		const auto index = opcode <= 0xff ? opcode : (opcode & 0xff) + 0x100;
		return instructions_[index];
	}

	[[nodiscard]] auto get_opcode(const uint16_t& starting_address, const Memory& memory) const -> uint16_t
	{
		const auto first_byte = static_cast<uint16_t>(memory.read(starting_address));
		if (first_byte == 0xcb) {
			const auto second_byte = memory.read(starting_address + 1);
			return static_cast<uint16_t>((first_byte << 8) + second_byte);
		}
		return first_byte;
	}

	// See https://meganesulli.com/generate-gb-opcodes/
	std::vector<Instruction> instructions_ = get_all_instructions();
	Registers regs_ = {};

};
