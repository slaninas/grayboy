#include "catch2/catch.hpp"

#include "cpu.h"

TEST_CASE("Half-carry Addition 8bit", "[cpu_utils]") {
	auto result = half_carry_add_8bit(0x00, 0x00);
	CHECK(result == false);

	result = half_carry_add_8bit(0x0f, 0x01);
	CHECK(result == true);

	result = half_carry_add_8bit(0xff, 0x01);
	CHECK(result == true);

	result = half_carry_add_8bit(0x07, 0x08);
	CHECK(result == false);

	result = half_carry_add_8bit(0x08, 0x08);
	CHECK(result == true);

	result = half_carry_add_8bit(0x0f, 0x0e);
	CHECK(result == true);
}

TEST_CASE("Half-carry Addition 16bit", "[cpu_utils]") {
	auto result = half_carry_add_16bit(0x0000, 0x0000);
	CHECK(result == false);

	result = half_carry_add_16bit(0x0f00, 0x0100);
	CHECK(result == true);

	result = half_carry_add_16bit(0xff00, 0x0100);
	CHECK(result == true);

	result = half_carry_add_16bit(0x0700, 0x0800);
	CHECK(result == false);

	result = half_carry_add_16bit(0x0800, 0x0800);
	CHECK(result == true);

	result = half_carry_add_16bit(0x0f00, 0x0e00);
	CHECK(result == true);
}

TEST_CASE("Carry Addition 8bit", "[cpu_utils]") {
	auto result = carry_add_8bit(0x00, 0x00);
	CHECK(result == false);

	result = carry_add_8bit(0xff, 0x01);
	CHECK(result == true);

	result = carry_add_8bit(0xfe, 0x01);
	CHECK(result == false);

	result = carry_add_8bit(0xfe, 0x02);
	CHECK(result == true);

	result = carry_add_8bit(0x08, 0x08);
	CHECK(result == false);

	result = carry_add_8bit(0xdd, 0xfe);
	CHECK(result == true);
}

TEST_CASE("Carry Addition 16bit", "[cpu_utils]") {
	auto result = carry_add_16bit(0x00, 0x00);
	CHECK(result == false);

	result = carry_add_16bit(0xffff, 0x01);
	CHECK(result == true);

	result = carry_add_16bit(0xfe00, 0x0100);
	CHECK(result == false);

	result = carry_add_16bit(0xfe00, 0x0200);
	CHECK(result == true);

	result = carry_add_16bit(0x08ff, 0x08ff);
	CHECK(result == false);

	result = carry_add_16bit(0xdd00, 0xfe00);
	CHECK(result == true);
}

TEST_CASE("Half-carry Subtract 8bit", "[cpu_utils]") {
	auto result = half_carry_sub_8bit(0x00, 0x00);
	CHECK(result == false);

	result = half_carry_sub_8bit(0x00, 0x01);
	CHECK(result == true);

	result = half_carry_sub_8bit(0xff, 0xff);
	CHECK(result == false);

	result = half_carry_sub_8bit(0xf7, 0x08);
	CHECK(result == true);
}

TEST_CASE("Half-carry Subtract 16bit", "[cpu_utils]") {
	auto result = half_carry_sub_16bit(0x0000, 0x0000);
	CHECK(result == false);

	result = half_carry_sub_16bit(0x0000, 0x0001);
	CHECK(result == true);

	result = half_carry_sub_16bit(0xff00, 0xff00);
	CHECK(result == false);

	result = half_carry_sub_16bit(0xf7ff, 0x08ff);
	CHECK(result == true);
}

TEST_CASE("Carry Subtract 8bit", "[cpu_utils]") {
	auto result = carry_sub_8bit(0x00, 0x00);
	CHECK(result == false);

	result = carry_sub_8bit(0x00, 0x01);
	CHECK(result == true);

	result = carry_sub_8bit(0x7f, 0x80);
	CHECK(result == true);

	result = carry_sub_8bit(0x3d, 0xa4);
	CHECK(result == true);
}

TEST_CASE("Carry Subtract 16bit", "[cpu_utils]") {
	auto result = carry_sub_16bit(0x0000, 0x0000);
	CHECK(result == false);

	result = carry_sub_16bit(0x0000, 0x0001);
	CHECK(result == true);

	result = carry_sub_16bit(0x7f00, 0x8000);
	CHECK(result == true);

	result = carry_sub_16bit(0x3d00, 0xa400);
	CHECK(result == true);
}
