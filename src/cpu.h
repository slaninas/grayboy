#pragma once

#include "instruction_utils.h"
#include "instructions.h"
#include "memory.h"
#include "registers.h"

#include <cassert>
#include <iostream>
#include <sstream>
#include <type_traits>
#include <vector>

struct DisassemblyInfo {
	uint16_t address;
	uint16_t next_address;
	Instruction instruction;
	std::vector<uint8_t> memory_representation;
};

[[nodiscard]] auto find_by_opcode(const uint16_t opcode, const std::vector<Instruction>& instructions) -> Instruction;

inline std::ofstream debug_log("debug_log");
class Cpu;
inline void log(const Cpu& cpu);

// Make exceptions asserts and run in debug
// TODO: Add unit tests
class Cpu {
public:
	using MemoryType = Memory;

	Cpu() = default;

	// TODO: Make move possible?
	Cpu(const MemoryType& memory) : memory_{memory} {}
	Cpu(const MemoryType& memory, const Registers& regs) : memory_{memory}, regs_{regs} {}

	void clear_registers()
	{
		regs_.clear();
	}

	[[nodiscard]] auto get_opcode(const uint16_t& starting_address) const
	{
		const auto first_byte = static_cast<uint16_t>(memory_.read(starting_address));
		if (first_byte == 0xcb) {
			const auto second_byte = memory_.read(starting_address + 1);
			return static_cast<uint16_t>((first_byte << 8) + second_byte);
		}
		return first_byte;
	}

	[[nodiscard]] auto execute_next()
	{
		// log(*this);
		const auto PC = regs_.read("PC");
		const auto opcode = get_opcode(PC);

		const auto instruction = find_by_opcode(opcode);

		const auto cycles = execute_opcode(instruction.opcode, PC, regs_, memory_);

		regs_.write("PC", regs_.read("PC") + instruction.size);

		return cycles;
	}

	[[nodiscard]] auto execute_opcode(const uint16_t& opcode, const uint16_t& PC, Registers& regs, Memory& memory) -> uint8_t;

	[[nodiscard]] auto memory_dump() const
	{
		return memory_.dump();
	}

	[[nodiscard]] auto registers() -> auto&
	{
		return regs_;
	}

	[[nodiscard]] auto registers() const  -> const auto&
	{
		return regs_;
	}

	// TODO: Remove this once I can do snapshots of whole cpu state
	[[nodiscard]] const auto& get_memory() const {
		return memory_;
	}

	[[nodiscard]] auto& get_memory() {
		return memory_;
	}

private:
	MemoryType memory_ = {};
	Registers regs_ = {};

	// See https://meganesulli.com/generate-gb-opcodes/
	std::vector<Instruction> instructions_ = get_all_instructions();

	[[nodiscard]] auto find_by_opcode(const uint16_t opcode) const -> Instruction
	{
		return ::find_by_opcode(opcode, instructions_);
	}

};


inline auto get_formatted(const int& value, const uint32_t& width) -> std::string {
	auto s = std::stringstream{};
	s << std::setfill('0') << std::setw(width) << std::hex << value;
	auto result = s.str();
	std::for_each(begin(result), end(result), [](auto& c) { c = std::toupper(c); });


	return result;
}

void log(const Cpu& cpu) {

	debug_log << std::hex;
	debug_log << "A: " << get_formatted(cpu.registers().read("A"), 2) << ' ';
	debug_log << "F: " << get_formatted(cpu.registers().read("F"), 2) << ' ';
	debug_log << "B: " << get_formatted(cpu.registers().read("B"), 2) << ' ';
	debug_log << "C: " << get_formatted(cpu.registers().read("C"), 2) << ' ';
	debug_log << "D: " << get_formatted(cpu.registers().read("D"), 2) << ' ';
	debug_log << "E: " << get_formatted(cpu.registers().read("E"), 2) << ' ';
	debug_log << "H: " << get_formatted(cpu.registers().read("H"), 2) << ' ';
	debug_log << "L: " << get_formatted(cpu.registers().read("L"), 2) << ' ';

	debug_log << "SP: " << get_formatted(cpu.registers().read("SP"), 4) << ' ';

	const auto PC = cpu.registers().read("PC");
	// TODO: Don't hardcode
	debug_log << "PC: 00:" << get_formatted(PC, 4) << ' ';
	debug_log << '(';
	debug_log << std::hex << get_formatted(cpu.get_memory().read(PC), 2) << ' ';
	debug_log << std::hex << get_formatted(cpu.get_memory().read(PC + 1), 2) << ' ';
	debug_log << std::hex << get_formatted(cpu.get_memory().read(PC + 2), 2) << ' ';
	debug_log << std::hex << get_formatted(cpu.get_memory().read(PC + 3), 2);
	debug_log << ')';
	debug_log << '\n';


}

