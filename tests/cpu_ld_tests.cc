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
	CHECK_THAT(cpu.registers_dump(), RegistersCompare{empty_regs});

	SECTION("Copy 0xEEFF into BC") {
		const auto cycles = cpu.execute_next();
		CHECK(cycles == 3);

		const auto correct_state = MakeRegisters{.B=0xEE, .C=0xFF, .PC=0x03}.get();
		CHECK_THAT(cpu.registers_dump(), RegistersCompare{correct_state});
	}

	SECTION("Copy 0xABCD into BC") {
		cpu.registers().write("PC", 0x03);
		const auto cycles = cpu.execute_next();
		CHECK(cycles == 3);

		const auto correct_state = MakeRegisters{.B=0xAB, .C=0xCD, .PC=0x06}.get();
		CHECK_THAT(cpu.registers_dump(), RegistersCompare{correct_state});
	}

	SECTION("Copy 0x0100 into BC") {
		cpu.registers().write("PC", 0x06);
		const auto cycles = cpu.execute_next();
		CHECK(cycles == 3);

		const auto correct_state = MakeRegisters{.B=0x01, .C=0x00, .PC=0x09}.get();
		CHECK_THAT(cpu.registers_dump(), RegistersCompare{correct_state});
	}

	// cpu.clear_registers();
	// TODO: Check this in tests for registers
	// CHECK_THAT(cpu.registers_dump(), RegistersCompare{empty_regs});
}

TEST_CASE("LD (BC), A - 0x02", "[ld]") {
	// TODO: Add comparator for memory, use it
	auto memory = Cpu::MemoryType{0x02};
	auto cpu = Cpu{std::move(memory)};

	SECTION("Copy A (0x12) from A into address held in BC (0x00)") {
		cpu.registers().write("A", 0x12);
		cpu.registers().write("BC", 0x00);

		const auto cycles = cpu.execute_next();
		CHECK(cycles == 2);

		const auto correct_memory = Cpu::MemoryType{0x12};
		CHECK(cpu.memory_dump() == correct_memory);

		const auto correct_registers = MakeRegisters{.A=0x12, .BC=0x00, .PC=0x01}.get();
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}

	SECTION("Copy A (0xBC) into address held in BC (0x04)") {
		cpu.registers().write("A", 0xBC);
		cpu.registers().write("BC", 0x04);

		const auto cycles = cpu.execute_next();
		CHECK(cycles == 2);

		const auto correct_memory = Cpu::MemoryType{0x02, 0x00, 0x00, 0x00, 0xBC};
		CHECK(cpu.memory_dump() == correct_memory);

		const auto correct_registers = MakeRegisters{.A=0xBC, .BC=0x04, .PC=0x01}.get();
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}

}
