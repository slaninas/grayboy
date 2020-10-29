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

TEST_CASE("INC DE - 0x13", "[arithmetic]") {
	const auto memory = MemoryChanger{{{0x00, 0x13}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.DE=0xab12, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	const auto correct_registers = RegistersChanger{.DE=0xab13, .PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
}

TEST_CASE("INC HL - 0x23", "[arithmetic]") {
	const auto memory = MemoryChanger{{{0x00, 0x23}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL=0xfeab, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	const auto correct_registers = RegistersChanger{.HL=0xfeac, .PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
}

TEST_CASE("INC SP - 0x33", "[arithmetic]") {
	const auto memory = MemoryChanger{{{0x00, 0x33}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC=0x00, .SP=0x1234}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	const auto correct_registers = RegistersChanger{.PC=0x01, .SP=0x1235}.get(orig_regs);
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
// TODO: Add memory check for INC instructions, it should not change
TEST_CASE("INC D - 0x14", "[arithmetic]") {
	const auto memory = MemoryChanger{{{0x00, 0x14}, {0x01, 0x14}, {0x02, 0x14}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.D=0x00, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	SECTION("From zero to three") {
		auto cycles = cpu.execute_next();
		CHECK(cycles == 1);
		auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=0}.get(orig_regs.read("F"));
		auto correct_registers = RegistersChanger{.F=correct_flags, .D=0x01, .PC=0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_flags = FlagsChanger{.Z=0, .N=0, .H=0}.get(correct_flags);
		correct_registers = RegistersChanger{.F=correct_flags, .D=0x02, .PC=0x02}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_flags = FlagsChanger{.Z=0, .N=0, .H=0}.get(correct_flags);
		correct_registers = RegistersChanger{.F=correct_flags, .D=0x03, .PC=0x03}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}
}

TEST_CASE("INC H - 0x24", "[arithmetic]") {
	const auto memory = MemoryChanger{{{0x00, 0x24}, {0x01, 0x24}, {0x02, 0x24}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.H=0x00, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	SECTION("From zero to three") {
		auto cycles = cpu.execute_next();
		CHECK(cycles == 1);
		auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=0}.get(orig_regs.read("F"));
		auto correct_registers = RegistersChanger{.F=correct_flags, .H=0x01, .PC=0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_flags = FlagsChanger{.Z=0, .N=0, .H=0}.get(correct_flags);
		correct_registers = RegistersChanger{.F=correct_flags, .H=0x02, .PC=0x02}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_flags = FlagsChanger{.Z=0, .N=0, .H=0}.get(correct_flags);
		correct_registers = RegistersChanger{.F=correct_flags, .H=0x03, .PC=0x03}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}
}

TEST_CASE("INC (HL) - 0x34", "[arithmetic]") {
	const auto memory = MemoryChanger{{{0x00, 0x34}, {0x01, 0x34}, {0x02, 0x34}, {0x1234, 0x00}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL=0x1234, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	SECTION("From zero to three") {
		auto cycles = cpu.execute_next();
		CHECK(cycles == 3);
		auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=0}.get(orig_regs.read("F"));
		auto correct_registers = RegistersChanger{.F=correct_flags, .PC=0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
		auto correct_memory = MemoryChanger{{{0x1234, 0x01}}}.get(memory);
		CHECK(cpu.memory_dump() == correct_memory.dump());

		cycles = cpu.execute_next();
		CHECK(cycles == 3);
		correct_flags = FlagsChanger{.Z=0, .N=0, .H=0}.get(correct_flags);
		correct_registers = RegistersChanger{.F=correct_flags, .PC=0x02}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
		correct_memory = MemoryChanger{{{0x1234, 0x02}}}.get(memory);
		CHECK(cpu.memory_dump() == correct_memory.dump());

		cycles = cpu.execute_next();
		CHECK(cycles == 3);
		correct_flags = FlagsChanger{.Z=0, .N=0, .H=0}.get(correct_flags);
		correct_registers = RegistersChanger{.F=correct_flags, .PC=0x03}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
		correct_memory = MemoryChanger{{{0x1234, 0x03}}}.get(memory);
		CHECK(cpu.memory_dump() == correct_memory.dump());
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

		auto correct_flags = FlagsChanger{.Z=0, .N=1, .H=0,}.get(orig_regs.read("F"));
		auto correct_registers = RegistersChanger{.F=correct_flags, .B=0x02, .PC=0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == memory.dump());

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_flags = FlagsChanger{.Z=0, .N=1, .H=0,}.get(correct_flags);
		correct_registers = RegistersChanger{.F=correct_flags, .B=0x01, .PC=0x02}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == memory.dump());

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_flags = FlagsChanger{.Z=1, .N=1, .H=0,}.get(correct_flags);
		correct_registers = RegistersChanger{.F=correct_flags, .B=0x00, .PC=0x03}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == memory.dump());
	}

	// TODO: More half-carry tests
	SECTION("Half-carry") {
		cpu.registers().write("B", 0xf0);
		const auto cycles = cpu.execute_next();
		CHECK(cycles == 1);

		const auto correct_flags = FlagsChanger{.Z=0, .N=1, .H=1,}.get(orig_regs.read("F"));
		const auto correct_registers = RegistersChanger{.F=correct_flags, .B=0xef, .PC=0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == memory.dump());
	}

}

// TODO: Does catch2 have some templates? This code is repeated, just register name is changed
TEST_CASE("DEC D - 0x15", "[arithmetic]") {
	const auto memory = MemoryChanger{{{0x00, 0x15}, {0x01, 0x15}, {0x02, 0x15}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.D=0x03, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	auto correct_flags = FlagsChanger{.Z=0, .N=1, .H=0,}.get(orig_regs.read("F"));
	auto correct_registers = RegistersChanger{.F=correct_flags, .D=0x02, .PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 1);
	correct_flags = FlagsChanger{.Z=0, .N=1, .H=0,}.get(correct_flags);
	correct_registers = RegistersChanger{.F=correct_flags, .D=0x01, .PC=0x02}.get(correct_registers);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 1);
	correct_flags = FlagsChanger{.Z=1, .N=1, .H=0,}.get(correct_flags);
	correct_registers = RegistersChanger{.F=correct_flags, .D=0x00, .PC=0x03}.get(correct_registers);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == memory.dump());
}

TEST_CASE("DEC H - 0x25", "[arithmetic]") {
	const auto memory = MemoryChanger{{{0x00, 0x25}, {0x01, 0x25}, {0x02, 0x25}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.H=0x03, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	auto correct_flags = FlagsChanger{.Z=0, .N=1, .H=0,}.get(orig_regs.read("F"));
	auto correct_registers = RegistersChanger{.F=correct_flags, .H=0x02, .PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 1);
	correct_flags = FlagsChanger{.Z=0, .N=1, .H=0,}.get(correct_flags);
	correct_registers = RegistersChanger{.F=correct_flags, .H=0x01, .PC=0x02}.get(correct_registers);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 1);
	correct_flags = FlagsChanger{.Z=1, .N=1, .H=0,}.get(correct_flags);
	correct_registers = RegistersChanger{.F=correct_flags, .H=0x00, .PC=0x03}.get(correct_registers);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == memory.dump());
}

TEST_CASE("DEC (HL) - 0x35", "[arithmetic]") {
	const auto memory = MemoryChanger{{{0x00, 0x35}, {0x01, 0x35}, {0x02, 0x35}, {0xabcd, 0x03}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL=0xabcd, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 3);

	auto correct_flags = FlagsChanger{.Z=0, .N=1, .H=0,}.get(orig_regs.read("F"));
	auto correct_registers = RegistersChanger{.F=correct_flags, .PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	auto correct_memory = MemoryChanger{{{0xabcd, 0x02}}}.get(memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 3);
	correct_flags = FlagsChanger{.Z=0, .N=1, .H=0,}.get(correct_flags);
	correct_registers = RegistersChanger{.F=correct_flags, .PC=0x02}.get(correct_registers);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	correct_memory = MemoryChanger{{{0xabcd, 0x01}}}.get(memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 3);
	correct_flags = FlagsChanger{.Z=1, .N=1, .H=0,}.get(correct_flags);
	correct_registers = RegistersChanger{.F=correct_flags, .PC=0x03}.get(correct_registers);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	correct_memory = MemoryChanger{{{0xabcd, 0x00}}}.get(memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
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
	const auto orig_regs = RegistersChanger{.C=0xee, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F=correct_flags, .C=0xef, .PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("DEC C - 0x0d", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x0d}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.C=0x01, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z=1, .N=1, .H=0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F=correct_flags, .C=0x00, .PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}
