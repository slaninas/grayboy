#include "catch2/catch.hpp"

#include "test_utils.h"

#include "cpu.h"

TEST_CASE("NOP - 0x00", "[misc]") {


	SECTION("Running with PC=0x00, NOP should just increase PC by one") {
		const auto orig_memory = MemoryChanger{{{0x00, 0x00}}}.get(getRandomMemory());
		const auto orig_regs = RegistersChanger{.PC=0x00}.get(getRandomRegisters());
		auto cpu = Cpu{orig_memory, orig_regs};

		const auto cycles = cpu.execute_next();
		CHECK(cycles == 1);

		const auto correct_registers = RegistersChanger{.PC=0x01}.get(orig_regs);;

		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}

	SECTION("Running with PC=0xBE, NOP should just increase PC by one") {
		const auto orig_memory = MemoryChanger{{{0xBE, 0x00}}}.get(getRandomMemory());
		const auto orig_regs = RegistersChanger{.PC=0xBE}.get(getRandomRegisters());
		auto cpu = Cpu{orig_memory, orig_regs};

		const auto cycles = cpu.execute_next();
		CHECK(cycles == 1);

		const auto correct_registers = RegistersChanger{.PC=0xBF}.get(orig_regs);;

		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}
}
