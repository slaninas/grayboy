#include "catch2/catch.hpp"

#include "cpu.h"

TEST_CASE("Half-carry 8bit", "[cpu_utils]") {
	const auto cpu = Cpu{};

	auto result = cpu.half_carry_add_8bit(0x00, 0x00);
	CHECK(result == false);

	result = cpu.half_carry_add_8bit(0x0f, 0x01);
	CHECK(result == true);

	result = cpu.half_carry_add_8bit(0xff, 0x01);
	CHECK(result == true);

	result = cpu.half_carry_add_8bit(0x07, 0x08);
	CHECK(result == false);

	result = cpu.half_carry_add_8bit(0x08, 0x08);
	CHECK(result == true);

	result = cpu.half_carry_add_8bit(0x0f, 0x0e);
	CHECK(result == true);
}

TEST_CASE("Half-carry 16bit", "[cpu_utils]") {
	const auto cpu = Cpu{};

	auto result = cpu.half_carry_add_8bit(0x0000, 0x0000);
	CHECK(result == false);

	result = cpu.half_carry_add_16bit(0x0f00, 0x0100);
	CHECK(result == true);

	result = cpu.half_carry_add_16bit(0xff00, 0x0100);
	CHECK(result == true);

	result = cpu.half_carry_add_16bit(0x0700, 0x0800);
	CHECK(result == false);

	result = cpu.half_carry_add_16bit(0x0800, 0x0800);
	CHECK(result == true);

	result = cpu.half_carry_add_16bit(0x0f00, 0x0e00);
	CHECK(result == true);
}
