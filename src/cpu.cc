#include "cpu.h"

#include <optional>

[[nodiscard]] auto find_by_opcode(const uint16_t opcode, const std::vector<Instruction>& instructions) -> Instruction
{
	const auto index = opcode <= 0xff ? opcode : (opcode & 0xff) + 0x100;
	return instructions[index];
}
