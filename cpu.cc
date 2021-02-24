#include "cpu.h"

#include <optional>

[[nodiscard]] auto find_by_opcode(const uint16_t opcode, const std::vector<Instruction>& instructions) -> Instruction {
	auto res = std::find_if(begin(instructions), end(instructions), [opcode](const auto& instruction) { return instruction.opcode ==  opcode; });
	// TODO(honza): Use assert when all instructions are implemented
	if (res == end(instructions)) {
		auto stream = std::stringstream{};
		stream << std::hex << opcode;
		throw std::runtime_error("Opcode 0x" + stream.str() + " not found.");
	}
	return *res;
}
