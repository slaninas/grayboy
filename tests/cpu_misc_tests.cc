#include "catch2/catch.hpp"

#include "test_utils.h"

#include "cpu.h"

TEST_CASE("NOP - 0x00", "[misc]") {
	auto cpu = Cpu{{}};

	// Fill registers with some values, we care about PC here
	const auto regs = MakeRegisters{.AF=0xAF, .BC=0xCB, .DE=0xDE, .HL=0x12, .PC=0x00, .SP=0x43}.get();
	cpu.registers() = regs;

	SECTION("Running with PC=0x00, NOP should just increase PC by one") {
		auto cycles = cpu.execute_next();
		CHECK(cycles ==  1);
		auto correct_registers = regs;
		correct_registers.write("PC", 0x01);

		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == Cpu::MemoryType{});
	}

	SECTION("Running with PC=0xBE, NOP should just increase PC by one") {
		cpu.registers().write("PC", 0xBE);
		auto cycles = cpu.execute_next();
		CHECK(cycles ==  1);
		auto correct_registers = regs;
		correct_registers.write("PC", 0xBF);

		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == Cpu::MemoryType{});
	}
}
