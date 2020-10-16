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
}
