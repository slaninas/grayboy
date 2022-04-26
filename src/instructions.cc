#include "instructions.h"

#include "instructions_16bit.h"
#include "instructions_8bit.h"

auto get_all_instructions() -> std::vector<Instruction>
{
	auto instructions_8bit = get_8bit_instructions();
	const auto instructions_16bit = get_16bit_instructions();

	std::copy(begin(instructions_16bit), end(instructions_16bit), std::back_inserter(instructions_8bit));

	auto sorted = std::vector<Instruction>(256 * 2);
	for (const auto& orig_instr : instructions_8bit) {
		const auto opcode = orig_instr.opcode <= 0xff ? orig_instr.opcode : (orig_instr.opcode & 0xff) + 0x100;
		auto& instr = sorted[opcode];
		instr.mnemonic = orig_instr.mnemonic;
		instr.opcode = orig_instr.opcode;
		instr.size = orig_instr.size;
		instr.execute = orig_instr.execute;
	}

	return sorted;
}
