#include "catch2/catch.hpp"

#include "test_utils.h"
#include "cpu.h"

TEST_CASE("LD BC, d16 - 0x01", "[ld]") {
	auto memory = Cpu::MemoryType{
		0x01, 0xEE, 0xFF,
		0x01, 0xAB, 0xCD,
		0x01, 0x01, 0x00
	};

	auto cpu = Cpu{std::move(memory)};

	const auto empty_regs = Registers{};
	// TODO: Check this in tests for registers
	CHECK_THAT(cpu.get_registers_dump(), RegistersCompare{empty_regs});

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 3);

	auto correct_state = Registers{};
	correct_state.B = 0xEE;
	correct_state.C = 0xFF;
	correct_state.PC = 0x03;
	CHECK_THAT(cpu.get_registers_dump(), RegistersCompare{correct_state});

	const auto cycles2 = cpu.execute_next();
	correct_state.clear();
	correct_state.B = 0xAB;
	correct_state.C = 0xCD;
	correct_state.PC = 0x06;
	CHECK_THAT(cpu.get_registers_dump(), RegistersCompare{correct_state});

	const auto cycles3 = cpu.execute_next();
	correct_state.clear();
	correct_state.B = 0x01;
	correct_state.C = 0x00;
	correct_state.PC = 0x09;
	CHECK_THAT(cpu.get_registers_dump(), RegistersCompare{correct_state});

	cpu.clear_registers();
	// TODO: Check this in tests for registers
	CHECK_THAT(cpu.get_registers_dump(), RegistersCompare{empty_regs});
}
