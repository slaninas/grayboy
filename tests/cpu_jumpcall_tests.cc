#include "catch2/catch.hpp"

#include "test_utils.h"

#include "cpu.h"

TEST_CASE("JR s8 - 0x18", "[jump/call]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x18}, {0x01, 0x43}, {0x43, 0x18}, {0x44, 0xde}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 3);
	auto correct_regs = RegistersChanger{.PC=0x43}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());


	cycles = cpu.execute_next();
	CHECK(cycles == 3);
	correct_regs = RegistersChanger{.PC=0x0121}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}
