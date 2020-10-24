#include "catch2/catch.hpp"

#include "test_utils.h"

#include "cpu.h"

// TODO: Fuzzy testing where it makes sense, even in other test files

TEST_CASE("INC BC - 0x03", "[arithmetic]") {
	// Increment BC three times from zero
	const auto memory = MemoryChanger{{{0x00, 0x03}, {0x01, 0x03}, {0x02, 0x03}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.BC=0x00, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	auto correct_registers = RegistersChanger{.BC=0x01, .PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

	cycles = cpu.execute_next();
	CHECK(cycles == 2);
	correct_registers = RegistersChanger{.BC=0x02, .PC=0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

	cycles = cpu.execute_next();
	CHECK(cycles == 2);
	correct_registers = RegistersChanger{.BC=0x03, .PC=0x03}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
}

TEST_CASE("INC B - 0x04", "[arithmetic]") {
	const auto memory = MemoryChanger{{{0x00, 0x04}, {0x01, 0x04}, {0x02, 0x04}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.BC=0x00, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	SECTION("From zero to three") {
		auto cycles = cpu.execute_next();
		CHECK(cycles == 1);
		auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=0}.get(orig_regs.read("F"));
		auto correct_registers = RegistersChanger{.F=correct_flags, .B=0x01, .PC=0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_flags = FlagsChanger{.Z=0, .N=0, .H=0}.get(correct_flags);
		correct_registers = RegistersChanger{.F=correct_flags, .B=0x02, .PC=0x02}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_flags = FlagsChanger{.Z=0, .N=0, .H=0}.get(correct_flags);
		correct_registers = RegistersChanger{.F=correct_flags, .B=0x03, .PC=0x03}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}

	SECTION("Z flag") {
		cpu.registers().write("B", 0xff);
		auto cycles = cpu.execute_next();
		CHECK(cycles == 1);

		const auto correct_flags = FlagsChanger{.Z=1, .N=0, .H=1,}.get(orig_regs.read("F"));
		const auto correct_registers = RegistersChanger{.F=correct_flags, .B=0x00, .PC=0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}

	SECTION("H flag") {
		cpu.registers().write("B", 0x0f);
		auto cycles = cpu.execute_next();
		CHECK(cycles == 1);

		auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=1,}.get(orig_regs.read("F"));
		auto correct_registers = RegistersChanger{.F=correct_flags, .B=0x10, .PC=0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_flags = FlagsChanger{.Z=0, .N=0, .H=0,}.get(correct_flags);
		correct_registers = RegistersChanger{.F=correct_flags, .B=0x11, .PC=0x02}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}

}

TEST_CASE("DEC B - 0x05", "[arithmetic]") {
	const auto memory = MemoryChanger{{{0x00, 0x05}, {0x01, 0x05}, {0x02, 0x05}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.B=0x0, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	SECTION("Three to zero") {
		cpu.registers().write("B", 0x03);
		auto cycles = cpu.execute_next();
		CHECK(cycles == 1);

		// C is unchanged, it's 1 just because it was 1 before
		auto correct_flags = FlagsChanger{.Z=0, .N=1, .H=0,}.get(orig_regs.read("F"));
		auto correct_registers = RegistersChanger{.F=correct_flags, .B=0x02, .PC=0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_flags = FlagsChanger{.Z=0, .N=1, .H=0,}.get(correct_flags);
		correct_registers = RegistersChanger{.F=correct_flags, .B=0x01, .PC=0x02}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_flags = FlagsChanger{.Z=1, .N=1, .H=0,}.get(correct_flags);
		correct_registers = RegistersChanger{.F=correct_flags, .B=0x00, .PC=0x03}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}

	// TODO: More half-carry tests
	SECTION("Half-carry") {
		cpu.registers().write("B", 0xf0);
		const auto cycles = cpu.execute_next();
		CHECK(cycles == 1);

		const auto correct_flags = FlagsChanger{.Z=0, .N=1, .H=1,}.get(orig_regs.read("F"));
		const auto correct_registers = RegistersChanger{.F=correct_flags, .B=0xef, .PC=0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}

}


TEST_CASE("ADD HL, BC - 0x09", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x09}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.BC=0xffee, .HL=0xabcd,.PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.N=0, .H=1, .C=1}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F=correct_flags, .HL=0xABBB, .PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("DEC BC - 0x0b", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x0b}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.BC=0xffee, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.BC=0xffed, .PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("INC C - 0x0c", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x0c}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.C=0xffee, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F=correct_flags, .C=0xffef, .PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}
