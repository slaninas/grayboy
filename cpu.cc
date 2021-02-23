#include "cpu.h"

#include <optional>

[[nodiscard]] Instruction find_by_opcode(const uint16_t opcode, const std::vector<Instruction>& instructions) {
	auto res = std::find_if(begin(instructions), end(instructions), [opcode](const auto& instruction) { return instruction.opcode ==  opcode; });
	if (res == end(instructions)) {
		auto stream = std::stringstream{};
		stream << std::hex << opcode;
		throw std::runtime_error("Opcode 0x" + stream.str() + " not found.");
	}
	return *res;
}
