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

TEST_CASE("SCF - 0x37", "[misc]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x37}, {0x01, 0x37}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C=0}.get(getRandomFlags());
	const auto orig_regs = RegistersChanger{.F=orig_flags, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	auto correct_flags = FlagsChanger{.C=1}.get(orig_flags);
	auto correct_registers = RegistersChanger{.F=correct_flags, .PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == orig_memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 1);
	correct_flags = FlagsChanger{.C=1}.get(orig_flags);
	correct_registers = RegistersChanger{.F=correct_flags, .PC=0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("CPL - 0x2f", "[misc]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x2f}, {0x01, 0x2f}}}.get(getRandomMemory());
	const auto orig_flags = getRandomFlags();
	const auto orig_regs = RegistersChanger{.A=0xb3, .F=orig_flags, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	auto correct_flags = FlagsChanger{.N=1, .H=1}.get(orig_flags);
	auto correct_registers = RegistersChanger{.A=0x4c, .F=correct_flags, .PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == orig_memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 1);
	correct_flags = FlagsChanger{.N=1, .H=1}.get(orig_flags);
	correct_registers = RegistersChanger{.A=0xb3, .F=correct_flags, .PC=0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}
