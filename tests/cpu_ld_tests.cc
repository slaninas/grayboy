#include "catch2/catch.hpp"

#include "test_utils.h"
#include "cpu.h"

TEST_CASE("LD BC, d16 - 0x01", "[ld]") {
	const auto orig_memory = MemoryChanger{{
		{0x00, 0x01}, {0x01, 0xEE}, {0x02, 0xFF},
		{0x03, 0x01}, {0x04, 0xAB}, {0x05, 0xCD},
		{0x06, 0x01}, {0x07, 0x01}, {0x08, 0x00}
	}}.get(getRandomMemory());

	const auto orig_regs = RegistersChanger{.PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	SECTION("Copy 0xEEFF into BC") {
		const auto cycles = cpu.execute_next();
		CHECK(cycles == 3);

		const auto correct_registers = RegistersChanger{.BC=0xEEFF, .PC=0x03}.get(orig_regs);
		CHECK_THAT(cpu.registers_dump(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}

	SECTION("Copy 0xABCD into BC") {
		cpu.registers().write("PC", 0x03);
		const auto cycles = cpu.execute_next();
		CHECK(cycles == 3);

		const auto correct_registers = RegistersChanger{.BC=0xABCD, .PC=0x06}.get(orig_regs);
		CHECK_THAT(cpu.registers_dump(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}

	SECTION("Copy 0x0100 into BC") {
		cpu.registers().write("PC", 0x06);
		const auto cycles = cpu.execute_next();
		CHECK(cycles == 3);

		const auto correct_registers = RegistersChanger{.BC=0x0100, .PC=0x09}.get(orig_regs);
		CHECK_THAT(cpu.registers_dump(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}
}

TEST_CASE("LD (BC), A - 0x02", "[ld]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x02}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	SECTION("Copy A (0x12) from A into address held in BC (0x00)") {
		cpu.registers().write("A", 0x12);
		cpu.registers().write("BC", 0x00);

		const auto cycles = cpu.execute_next();
		CHECK(cycles == 2);

		const auto correct_memory = MemoryChanger{{{0x00, 0x12}}}.get(orig_memory);
		CHECK(cpu.memory_dump() == correct_memory.dump());

		const auto correct_registers = RegistersChanger{.A=0x12, .BC=0x00, .PC=0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}

	SECTION("Copy A (0xBC) into address held in BC (0x04)") {
		cpu.registers().write("A", 0xBC);
		cpu.registers().write("BC", 0x04);

		const auto cycles = cpu.execute_next();
		CHECK(cycles == 2);

		const auto correct_memory = MemoryChanger{{{0x04, 0xBC}}}.get(orig_memory);
		CHECK(cpu.memory_dump() == correct_memory.dump());

		const auto correct_registers = RegistersChanger{.A=0xBC, .BC=0x04, .PC=0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}
}
