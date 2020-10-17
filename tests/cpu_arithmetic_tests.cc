#include "catch2/catch.hpp"

#include "test_utils.h"

#include "cpu.h"

TEST_CASE("INC BC - 0x03", "[arithmetic]") {
	// Increment BC three times from zero
	auto cpu = Cpu{{0x03, 0x03, 0x03}};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	auto correct_registers = MakeRegisters{.BC=0x01, .PC=0x01}.get();
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

	cycles = cpu.execute_next();
	CHECK(cycles == 2);
	correct_registers = MakeRegisters{.BC=0x02, .PC=0x02}.get();
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

	cycles = cpu.execute_next();
	CHECK(cycles == 2);
	correct_registers = MakeRegisters{.BC=0x03, .PC=0x03}.get();
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
}
