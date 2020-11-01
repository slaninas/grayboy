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

TEST_CASE("JR Z, s8 - 0x28", "[jump/call]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x28}, {0x01, 0x43}, {0x02, 0x28}, {0x03, 0xde}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.Z=0}.get(getRandomFlags());
	const auto orig_regs = RegistersChanger{.F=orig_flags, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	// Z is 0 so this should continue to the next instruction
	auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	auto correct_regs = RegistersChanger{.PC=0x2}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());


	cpu.registers().set_flag("Z", true);
	cycles = cpu.execute_next();
	CHECK(cycles == 3);
	const auto correct_flags = FlagsChanger{.Z=1}.get(orig_flags); // Z was manually setg to true
	correct_regs = RegistersChanger{.F=correct_flags, .PC=0xe0}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("JR C, s8 - 0x38", "[jump/call]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x38}, {0x01, 0x43}, {0x02, 0x38}, {0x03, 0xde}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C=0}.get(getRandomFlags());
	const auto orig_regs = RegistersChanger{.F=orig_flags, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	// Z is 0 so this should continue to the next instruction
	auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	auto correct_regs = RegistersChanger{.PC=0x2}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());


	cpu.registers().set_flag("C", true);
	cycles = cpu.execute_next();
	CHECK(cycles == 3);
	const auto correct_flags = FlagsChanger{.C=1}.get(orig_flags); // C was manually setg to true
	correct_regs = RegistersChanger{.F=correct_flags, .PC=0xe0}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("RET NZ - 0xc0", "[jump/call]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xc0}, {0x1234, 0xcb}, {0x1235, 0xae}}}.get(getRandomMemory());

	SECTION("Do return") {
		const auto orig_flags = FlagsChanger{.C=0}.get(getRandomFlags());
		const auto orig_regs = RegistersChanger{.F=orig_flags, .PC=0x00, .SP=0x1234}.get(getRandomRegisters());
		auto cpu = Cpu{orig_memory, orig_regs};

		auto cycles = cpu.execute_next();
		CHECK(cycles == 5);
		auto correct_regs = RegistersChanger{.PC=0xaecb, .SP=0x1236}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}

	SECTION("Do not return") {
		const auto orig_flags = FlagsChanger{.C=1}.get(getRandomFlags());
		const auto orig_regs = RegistersChanger{.F=orig_flags, .PC=0x00, .SP=0x1234}.get(getRandomRegisters());
		auto cpu = Cpu{orig_memory, orig_regs};

		auto cycles = cpu.execute_next();
		CHECK(cycles == 2);
		auto correct_regs = RegistersChanger{.PC=0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}
}
