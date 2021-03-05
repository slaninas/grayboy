#include "catch2/catch.hpp"
#include "cpu.h"
#include "test_utils.h"

// TODO: Fuzzy testing where it makes sense, even in other test files

TEST_CASE("INC BC - 0x03", "[arithmetic]") {
	// Increment BC three times from zero
	const auto memory = MemoryChanger{{{0x00, 0x03}, {0x01, 0x03}, {0x02, 0x03}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.BC = 0x00, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	auto correct_registers = RegistersChanger{.BC = 0x01, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

	cycles = cpu.execute_next();
	CHECK(cycles == 2);
	correct_registers = RegistersChanger{.BC = 0x02, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

	cycles = cpu.execute_next();
	CHECK(cycles == 2);
	correct_registers = RegistersChanger{.BC = 0x03, .PC = 0x03}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
}

TEST_CASE("INC DE - 0x13", "[arithmetic]") {
	const auto memory = MemoryChanger{{{0x00, 0x13}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.DE = 0xab12, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	const auto correct_registers = RegistersChanger{.DE = 0xab13, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
}

TEST_CASE("INC HL - 0x23", "[arithmetic]") {
	const auto memory = MemoryChanger{{{0x00, 0x23}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL = 0xfeab, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	const auto correct_registers = RegistersChanger{.HL = 0xfeac, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
}

TEST_CASE("INC SP - 0x33", "[arithmetic]") {
	const auto memory = MemoryChanger{{{0x00, 0x33}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00, .SP = 0x1234}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	const auto correct_registers = RegistersChanger{.PC = 0x01, .SP = 0x1235}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
}

TEST_CASE("INC B - 0x04", "[arithmetic]") {
	const auto memory = MemoryChanger{{{0x00, 0x04}, {0x01, 0x04}, {0x02, 0x04}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.BC = 0x00, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	SECTION("From zero to three") {
		auto cycles = cpu.execute_next();
		CHECK(cycles == 1);
		auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0}.get(orig_regs.read("F"));
		auto correct_registers = RegistersChanger{.F = correct_flags, .B = 0x01, .PC = 0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0}.get(correct_flags);
		correct_registers = RegistersChanger{.F = correct_flags, .B = 0x02, .PC = 0x02}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0}.get(correct_flags);
		correct_registers = RegistersChanger{.F = correct_flags, .B = 0x03, .PC = 0x03}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}

	SECTION("Z flag") {
		cpu.registers().write("B", 0xff);
		auto cycles = cpu.execute_next();
		CHECK(cycles == 1);

		const auto correct_flags =
		  FlagsChanger{
		    .Z = 1,
		    .N = 0,
		    .H = 1,
		  }
		    .get(orig_regs.read("F"));
		const auto correct_registers = RegistersChanger{.F = correct_flags, .B = 0x00, .PC = 0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}

	SECTION("H flag") {
		cpu.registers().write("B", 0x0f);
		auto cycles = cpu.execute_next();
		CHECK(cycles == 1);

		auto correct_flags =
		  FlagsChanger{
		    .Z = 0,
		    .N = 0,
		    .H = 1,
		  }
		    .get(orig_regs.read("F"));
		auto correct_registers = RegistersChanger{.F = correct_flags, .B = 0x10, .PC = 0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_flags =
		  FlagsChanger{
		    .Z = 0,
		    .N = 0,
		    .H = 0,
		  }
		    .get(correct_flags);
		correct_registers = RegistersChanger{.F = correct_flags, .B = 0x11, .PC = 0x02}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}
}
// TODO: Add memory check for INC instructions, it should not change
TEST_CASE("INC D - 0x14", "[arithmetic]") {
	const auto memory = MemoryChanger{{{0x00, 0x14}, {0x01, 0x14}, {0x02, 0x14}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.D = 0x00, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	SECTION("From zero to three") {
		auto cycles = cpu.execute_next();
		CHECK(cycles == 1);
		auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0}.get(orig_regs.read("F"));
		auto correct_registers = RegistersChanger{.F = correct_flags, .D = 0x01, .PC = 0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0}.get(correct_flags);
		correct_registers = RegistersChanger{.F = correct_flags, .D = 0x02, .PC = 0x02}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0}.get(correct_flags);
		correct_registers = RegistersChanger{.F = correct_flags, .D = 0x03, .PC = 0x03}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}
}

TEST_CASE("INC H - 0x24", "[arithmetic]") {
	const auto memory = MemoryChanger{{{0x00, 0x24}, {0x01, 0x24}, {0x02, 0x24}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.H = 0x00, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	SECTION("From zero to three") {
		auto cycles = cpu.execute_next();
		CHECK(cycles == 1);
		auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0}.get(orig_regs.read("F"));
		auto correct_registers = RegistersChanger{.F = correct_flags, .H = 0x01, .PC = 0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0}.get(correct_flags);
		correct_registers = RegistersChanger{.F = correct_flags, .H = 0x02, .PC = 0x02}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0}.get(correct_flags);
		correct_registers = RegistersChanger{.F = correct_flags, .H = 0x03, .PC = 0x03}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}
}

TEST_CASE("INC (HL) - 0x34", "[arithmetic]") {
	const auto memory =
	  MemoryChanger{{{0x00, 0x34}, {0x01, 0x34}, {0x02, 0x34}, {0x1234, 0x00}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL = 0x1234, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	SECTION("From zero to three") {
		auto cycles = cpu.execute_next();
		CHECK(cycles == 3);
		auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0}.get(orig_regs.read("F"));
		auto correct_registers = RegistersChanger{.F = correct_flags, .PC = 0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
		auto correct_memory = MemoryChanger{{{0x1234, 0x01}}}.get(memory);
		CHECK(cpu.memory_dump() == correct_memory.dump());

		cycles = cpu.execute_next();
		CHECK(cycles == 3);
		correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0}.get(correct_flags);
		correct_registers = RegistersChanger{.F = correct_flags, .PC = 0x02}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
		correct_memory = MemoryChanger{{{0x1234, 0x02}}}.get(memory);
		CHECK(cpu.memory_dump() == correct_memory.dump());

		cycles = cpu.execute_next();
		CHECK(cycles == 3);
		correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0}.get(correct_flags);
		correct_registers = RegistersChanger{.F = correct_flags, .PC = 0x03}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
		correct_memory = MemoryChanger{{{0x1234, 0x03}}}.get(memory);
		CHECK(cpu.memory_dump() == correct_memory.dump());
	}
}

TEST_CASE("DEC B - 0x05", "[arithmetic]") {
	const auto memory = MemoryChanger{{{0x00, 0x05}, {0x01, 0x05}, {0x02, 0x05}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.B = 0x0, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	SECTION("Three to zero") {
		cpu.registers().write("B", 0x03);
		auto cycles = cpu.execute_next();
		CHECK(cycles == 1);

		auto correct_flags =
		  FlagsChanger{
		    .Z = 0,
		    .N = 1,
		    .H = 0,
		  }
		    .get(orig_regs.read("F"));
		auto correct_registers = RegistersChanger{.F = correct_flags, .B = 0x02, .PC = 0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == memory.dump());

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_flags =
		  FlagsChanger{
		    .Z = 0,
		    .N = 1,
		    .H = 0,
		  }
		    .get(correct_flags);
		correct_registers = RegistersChanger{.F = correct_flags, .B = 0x01, .PC = 0x02}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == memory.dump());

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_flags =
		  FlagsChanger{
		    .Z = 1,
		    .N = 1,
		    .H = 0,
		  }
		    .get(correct_flags);
		correct_registers = RegistersChanger{.F = correct_flags, .B = 0x00, .PC = 0x03}.get(correct_registers);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == memory.dump());
	}

	// TODO: More half-carry tests
	SECTION("Half-carry") {
		cpu.registers().write("B", 0xf0);
		const auto cycles = cpu.execute_next();
		CHECK(cycles == 1);

		const auto correct_flags =
		  FlagsChanger{
		    .Z = 0,
		    .N = 1,
		    .H = 1,
		  }
		    .get(orig_regs.read("F"));
		const auto correct_registers = RegistersChanger{.F = correct_flags, .B = 0xef, .PC = 0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == memory.dump());
	}
}

// TODO: Does catch2 have some templates? This code is repeated, just register name is changed
TEST_CASE("DEC D - 0x15", "[arithmetic]") {
	const auto memory = MemoryChanger{{{0x00, 0x15}, {0x01, 0x15}, {0x02, 0x15}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.D = 0x03, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	auto correct_flags =
	  FlagsChanger{
	    .Z = 0,
	    .N = 1,
	    .H = 0,
	  }
	    .get(orig_regs.read("F"));
	auto correct_registers = RegistersChanger{.F = correct_flags, .D = 0x02, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 1);
	correct_flags =
	  FlagsChanger{
	    .Z = 0,
	    .N = 1,
	    .H = 0,
	  }
	    .get(correct_flags);
	correct_registers = RegistersChanger{.F = correct_flags, .D = 0x01, .PC = 0x02}.get(correct_registers);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 1);
	correct_flags =
	  FlagsChanger{
	    .Z = 1,
	    .N = 1,
	    .H = 0,
	  }
	    .get(correct_flags);
	correct_registers = RegistersChanger{.F = correct_flags, .D = 0x00, .PC = 0x03}.get(correct_registers);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == memory.dump());
}

TEST_CASE("DEC H - 0x25", "[arithmetic]") {
	const auto memory = MemoryChanger{{{0x00, 0x25}, {0x01, 0x25}, {0x02, 0x25}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.H = 0x03, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	auto correct_flags =
	  FlagsChanger{
	    .Z = 0,
	    .N = 1,
	    .H = 0,
	  }
	    .get(orig_regs.read("F"));
	auto correct_registers = RegistersChanger{.F = correct_flags, .H = 0x02, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 1);
	correct_flags =
	  FlagsChanger{
	    .Z = 0,
	    .N = 1,
	    .H = 0,
	  }
	    .get(correct_flags);
	correct_registers = RegistersChanger{.F = correct_flags, .H = 0x01, .PC = 0x02}.get(correct_registers);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 1);
	correct_flags =
	  FlagsChanger{
	    .Z = 1,
	    .N = 1,
	    .H = 0,
	  }
	    .get(correct_flags);
	correct_registers = RegistersChanger{.F = correct_flags, .H = 0x00, .PC = 0x03}.get(correct_registers);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == memory.dump());
}

TEST_CASE("DEC (HL) - 0x35", "[arithmetic]") {
	const auto memory =
	  MemoryChanger{{{0x00, 0x35}, {0x01, 0x35}, {0x02, 0x35}, {0xabcd, 0x03}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL = 0xabcd, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 3);

	auto correct_flags =
	  FlagsChanger{
	    .Z = 0,
	    .N = 1,
	    .H = 0,
	  }
	    .get(orig_regs.read("F"));
	auto correct_registers = RegistersChanger{.F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	auto correct_memory = MemoryChanger{{{0xabcd, 0x02}}}.get(memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 3);
	correct_flags =
	  FlagsChanger{
	    .Z = 0,
	    .N = 1,
	    .H = 0,
	  }
	    .get(correct_flags);
	correct_registers = RegistersChanger{.F = correct_flags, .PC = 0x02}.get(correct_registers);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	correct_memory = MemoryChanger{{{0xabcd, 0x01}}}.get(memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 3);
	correct_flags =
	  FlagsChanger{
	    .Z = 1,
	    .N = 1,
	    .H = 0,
	  }
	    .get(correct_flags);
	correct_registers = RegistersChanger{.F = correct_flags, .PC = 0x03}.get(correct_registers);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	correct_memory = MemoryChanger{{{0xabcd, 0x00}}}.get(memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("ADD HL, BC - 0x09", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x09}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.BC = 0xffee, .HL = 0xabcd, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.N = 0, .H = 1, .C = 1}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .HL = 0xABBB, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADD HL, DE - 0x19", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x19}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.DE = 0x1234, .HL = 0x5678, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .HL = 0x68ac, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADD HL, HL - 0x29", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x29}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL = 0xabcd, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.N = 0, .H = 1, .C = 1}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .HL = 0x579a, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADD HL, SP - 0x39", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x39}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL = 0x9876, .PC = 0x00, .SP = 0x1234}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .HL = 0xaaaa, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("DEC BC - 0x0b", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x0b}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.BC = 0xffee, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.BC = 0xffed, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("DEC DE - 0x1b", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x1b}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.DE = 0xabcd, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.DE = 0xabcc, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("DEC HL - 0x2b", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x2b}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL = 0xffff, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.HL = 0xfffe, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("DEC SP - 0x3b", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x3b}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00, .SP = 0x2109}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.PC = 0x01, .SP = 0x2108}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("INC C - 0x0c", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x0c}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.C = 0xee, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .C = 0xef, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("INC E - 0x1c", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x1c}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.E = 0xff, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 1, .N = 0, .H = 1}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .E = 0x00, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("INC L - 0x2c", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x2c}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.L = 0xaf, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 1}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .L = 0xb0, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("INC A - 0x3c", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x3c}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xf7, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0xf8, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("DEC C - 0x0d", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x0d}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.C = 0x01, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 1, .N = 1, .H = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .C = 0x00, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("DEC E - 0x1d", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x1d}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.E = 0xff, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .E = 0xfe, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("DEC L - 0x2d", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x2d}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.L = 0x00, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 1}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .L = 0xff, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("DEC A - 0x3d", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x3d}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x56, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x55, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

// TODO: 0x8*, 0x9*, 0xa* and 0xb* are mostly copy-pasted with just register changed, rewrite?
// ----------------------- 0x8* -----------------------
TEST_CASE("ADD A, B - 0x80", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x80}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x56, .B = 0x12, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x68, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADD A, C - 0x81", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x81}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x12, .C = 0x34, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x46, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADD A, D - 0x82", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x82}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x12, .D = 0x34, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x46, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADD A, E - 0x83", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x83}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x12, .E = 0x34, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x46, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADD A, H - 0x84", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x84}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x12, .H = 0x34, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x46, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADD A, L - 0x85", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x85}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x12, .L = 0x34, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x46, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADD A, (HL) - 0x86", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x86}, {0xbcda, 0x34}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x12, .HL = 0xbcda, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x46, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADD A, A - 0x87", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x87}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x12, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x24, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADC A, B - 0x88", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x88}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C = 1}.get(getRandomFlags());
	const auto orig_regs =
	  RegistersChanger{.A = 0x12, .F = orig_flags, .B = 0x0f, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 1, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x22, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADC A, C - 0x89", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x89}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C = 1}.get(getRandomFlags());
	const auto orig_regs =
	  RegistersChanger{.A = 0x12, .F = orig_flags, .C = 0x0f, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 1, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x22, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADC A, D - 0x8a", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x8a}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C = 1}.get(getRandomFlags());
	const auto orig_regs =
	  RegistersChanger{.A = 0x12, .F = orig_flags, .D = 0x0f, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 1, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x22, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADC A, E - 0x8b", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x8b}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C = 1}.get(getRandomFlags());
	const auto orig_regs =
	  RegistersChanger{.A = 0x12, .F = orig_flags, .E = 0x0f, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 1, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x22, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADC A, H - 0x8c", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x8c}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C = 1}.get(getRandomFlags());
	const auto orig_regs =
	  RegistersChanger{.A = 0x12, .F = orig_flags, .H = 0x0f, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 1, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x22, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADC A, L - 0x8d", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x8d}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C = 1}.get(getRandomFlags());
	const auto orig_regs =
	  RegistersChanger{.A = 0x12, .F = orig_flags, .L = 0x0f, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 1, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x22, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADC A, (HL) - 0x8e", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x8e}, {0x1234, 0x0f}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C = 1}.get(getRandomFlags());
	const auto orig_regs =
	  RegistersChanger{.A = 0x12, .F = orig_flags, .HL = 0x1234, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 1, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x22, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADC A, A - 0x8f", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x8f}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C = 1}.get(getRandomFlags());
	const auto orig_regs = RegistersChanger{.A = 0x12, .F = orig_flags, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x25, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADC A, d8 - 0xce", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xce}, {0x01, 0x0f}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C = 1}.get(getRandomFlags());
	const auto orig_regs = RegistersChanger{.A = 0x12, .F = orig_flags, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 1, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x22, .F = correct_flags, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("SUB B - 0x90", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x90}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x56, .B = 0x12, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x44, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("SUB C - 0x91", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x91}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x56, .C = 0x12, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x44, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("SUB D - 0x92", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x92}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x56, .D = 0x12, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x44, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("SUB E - 0x93", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x93}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x56, .E = 0x12, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x44, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("SUB H - 0x94", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x94}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x56, .H = 0x12, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x44, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("SUB L - 0x95", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x95}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x56, .L = 0x12, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x44, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("SUB (HL) - 0x96", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x96}, {0xbcaa, 0x12}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x56, .HL = 0xbcaa, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x44, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("SUB A - 0x97", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x97}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x56, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 1, .N = 1, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x00, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("SBC A, B - 0x98", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x98}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C = 1}.get(getRandomFlags());
	const auto orig_regs =
	  RegistersChanger{.A = 0x56, .F = orig_flags, .B = 0x34, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x21, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("SBC A, C - 0x99", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x99}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C = 1}.get(getRandomFlags());
	const auto orig_regs =
	  RegistersChanger{.A = 0x56, .F = orig_flags, .C = 0x34, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x21, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("SBC A, D - 0x9a", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x9a}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C = 1}.get(getRandomFlags());
	const auto orig_regs =
	  RegistersChanger{.A = 0x56, .F = orig_flags, .D = 0x34, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x21, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("SBC A, E - 0x9b", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x9b}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C = 1}.get(getRandomFlags());
	const auto orig_regs =
	  RegistersChanger{.A = 0x56, .F = orig_flags, .E = 0x34, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x21, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("SBC A, H - 0x9c", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x9c}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C = 1}.get(getRandomFlags());
	const auto orig_regs =
	  RegistersChanger{.A = 0x56, .F = orig_flags, .H = 0x34, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x21, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("SBC A, L - 0x9d", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x9d}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C = 1}.get(getRandomFlags());
	const auto orig_regs =
	  RegistersChanger{.A = 0x56, .F = orig_flags, .L = 0x34, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x21, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("SBC A, (HL) - 0x9e", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x9e}, {0xbcda, 0x34}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C = 1}.get(getRandomFlags());
	const auto orig_regs =
	  RegistersChanger{.A = 0x56, .F = orig_flags, .HL = 0xbcda, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x21, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("SBC A, A - 0x9f", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x9f}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C = 1}.get(getRandomFlags());
	const auto orig_regs = RegistersChanger{.A = 0x00, .F = orig_flags, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 1, .C = 1}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0xff, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("SBC A, d8 - 0xde", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xde}, {0x01, 0x34}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C = 1}.get(getRandomFlags());
	const auto orig_regs =
	  RegistersChanger{.A = 0x56, .F = orig_flags, .L = 0x34, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x21, .F = correct_flags, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("AND A, B - 0xa0", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xa0}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .B = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 1, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0xc3, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("AND A, C - 0xa1", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xa1}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .C = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 1, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0xc3, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("AND A, D - 0xa2", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xa2}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .D = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 1, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0xc3, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("AND A, E - 0xa3", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xa3}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .E = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 1, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0xc3, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("AND A, H - 0xa4", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xa4}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .H = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 1, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0xc3, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("AND A, L - 0xa5", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xa5}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .L = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 1, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0xc3, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("AND A, (HL) - 0xa6", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xa6}, {0xabcd, 0xf3}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .HL = 0xabcd, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 1, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0xc3, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("AND A, A - 0xa7", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xa7}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 1, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

// ----------------------- XOR -----------------------

TEST_CASE("XOR A, B - 0xa8", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xa8}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .B = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x38, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("XOR A, C - 0xa9", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xa9}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .C = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x38, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("XOR A, D - 0xaa", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xaa}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .D = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x38, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("XOR A, E - 0xab", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xab}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .E = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x38, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("XOR A, H - 0xac", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xac}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .H = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x38, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("XOR A, L - 0xad", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xad}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .L = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x38, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("XOR A, (HL) - 0xae", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xae}, {0xabcd, 0xf3}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .HL = 0xabcd, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x38, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("XOR A, A - 0xaf", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xaf}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 1, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x00, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("XOR d8 - 0xee", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xee}, {0x01, 0xf3}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x38, .F = correct_flags, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

// ----------------------- OR -----------------------

TEST_CASE("OR B - 0xb0", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xb0}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .B = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0xfb, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("OR C - 0xb1", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xb1}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .C = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0xfb, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("OR D - 0xb2", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xb2}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .D = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0xfb, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("OR E - 0xb3", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xb3}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .E = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0xfb, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("OR H - 0xb4", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xb4}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .H = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0xfb, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("OR L - 0xb5", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xb5}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .L = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0xfb, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("OR (HL) - 0xb6", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xb6}, {0xabcd, 0xf3}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .HL = 0xabcd, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0xfb, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("OR A - 0xb7", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xb7}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xfb, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0xfb, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

// ----------------------- CP -----------------------

TEST_CASE("CP B - 0xb8", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xb8}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .B = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 1}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("CP C - 0xb9", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xb9}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .C = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 1}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("CP D - 0xba", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xba}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .D = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 1}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("CP E - 0xbb", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xbb}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .E = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 1}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("CP H - 0xbc", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xbc}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .H = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 1}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("CP L - 0xbd", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xbd}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .L = 0xf3, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 1}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("CP (HL) - 0xbe", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xbe}, {0xabcd, 0xf3}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .HL = 0xabcd, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 1}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("CP A - 0xbf", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xbf}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xfb, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_flags = FlagsChanger{.Z = 1, .N = 1, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("CP d8 - 0xfe", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xfe}, {0x01, 0xf3}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xcb, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 0, .C = 1}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADD A, d8 - 0xc6", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xc6}, {0x01, 0xba}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x38, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 1, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0xF2, .F = correct_flags, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("SUB A, d8 - 0xd6", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xd6}, {0x01, 0xba}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x38, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 1, .H = 1, .C = 1}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x7e, .F = correct_flags, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("AND d8 - 0xe6", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xe6}, {0x01, 0xba}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x38, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 1, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0x38, .F = correct_flags, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("OR d8 - 0xf6", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xf6}, {0x01, 0xba}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xba, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.A = 0xba, .F = correct_flags, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("ADD SP, s8 - 0xe8", "[arithmetic]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xe8}, {0x01, 0x04}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00, .SP = 0x1234}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 4);

	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_regs.read("F"));
	const auto correct_regs = RegistersChanger{.F = correct_flags, .PC = 0x02, .SP = 0x1238}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}
