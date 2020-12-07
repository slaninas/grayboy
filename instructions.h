#pragma once

#include "instructions_8bit.h"
#include "instructions_16bit.h"

// TODO: Make functions in headers inline or put definitions into .cc files
// TODO: Add function to check which instructions are not implemented yet

auto get_all_instructions() {
	auto instructions_8bit = get_8bit_instructions();
	const auto instructions_16bit = get_16bit_instructions();

	std::copy(begin(instructions_16bit), end(instructions_16bit), std::back_inserter(instructions_8bit));

	return instructions_8bit;
}
