#include "instructions.h"

#include "instructions_16bit.h"
#include "instructions_8bit.h"

auto get_all_instructions() -> std::vector<Instruction> {
	auto instructions_8bit = get_8bit_instructions();
	const auto instructions_16bit = get_16bit_instructions();

	std::copy(begin(instructions_16bit), end(instructions_16bit), std::back_inserter(instructions_8bit));

	return instructions_8bit;
}

void check_implemented() {
	std::cout << std::hex;
	const auto omitted_8bit_opcodes =
	  std::vector<uint8_t>{0xcb, 0xd3, 0xdb, 0xdd, 0xe3, 0xe4, 0xeb, 0xec, 0xed, 0xf4, 0xfc, 0xfd};

	// Check 8bit opcodes
	const auto all_8bit = get_8bit_instructions();
	for (uint16_t opcode = 0x00; opcode <= 0xff; ++opcode) {
		const auto is_implemented = std::find_if(cbegin(all_8bit), cend(all_8bit), [opcode](const auto& instruction) {
			                            return instruction.opcode == opcode;
		                            }) != cend(all_8bit);

		const auto should_be_implemented =
		  std::find_if(
		    cbegin(omitted_8bit_opcodes), cend(omitted_8bit_opcodes), [opcode](const auto& instruction_opcode) {
			    return instruction_opcode == opcode;
		    }) == cend(omitted_8bit_opcodes);

		if (should_be_implemented && !is_implemented) {
			std::cout << "Instruction 0x" << static_cast<int>(opcode)
			          << " not found among implemented 8bit instructions.\n";
		}
		else if (!should_be_implemented && is_implemented) {
			std::cout << "Instruction 0x" << static_cast<int>(opcode) << " is implemented but should not be.\n";
		}
	}

	const auto all_16bit = get_16bit_instructions();

	for (uint16_t opcode = 0xcb00; opcode <= 0xcbff; ++opcode) {
		const auto is_implemented = std::find_if(cbegin(all_16bit), cend(all_16bit), [opcode](const auto& instruction) {
			                            return instruction.opcode == opcode;
		                            }) != cend(all_16bit);

		if (!is_implemented) {
			std::cout << "Instruction 0x" << static_cast<int>(opcode)
			          << " not found among implemented 16bit instructions.\n";
		}
	}

	std::cout << std::dec;
}