#include "catch2/catch.hpp"

#include "test_utils.h"
#include "cpu.h"


TEST_CASE("LD BC, d16", "[ld]") {
	auto memory = Cpu::MemoryType{0x01, 0xEE, 0xFF, 0x11};

	auto cpu = Cpu{std::move(memory)};

	cpu.get_registers_dump().print();

	auto empty_regs = Registers{};
	// TODO: Check this in tests for registers
	CHECK_THAT(cpu.get_registers_dump(), RegistersCompare{empty_regs});

	cpu.execute_next();
	cpu.get_registers_dump().print();

	auto correct_state = Registers{};
	correct_state.B = 0xEE;
	correct_state.C = 0xFF;
	correct_state.PC == 0x03;
	CHECK_THAT(cpu.get_registers_dump(), RegistersCompare{correct_state});


}
