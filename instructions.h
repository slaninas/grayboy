#pragma once

#include "instructions_8bit.h"
#include "instructions_16bit.h"

auto get_all_instructions() {
	auto instructions_8bit = get_8bit_instructions();
	const auto instructions_16bit = get_16bit_instructions();

	std::copy(begin(instructions_16bit), end(instructions_16bit), std::back_inserter(instructions_8bit));
	std::cout << instructions_8bit.size() << '\n';

	return instructions_8bit;
}
