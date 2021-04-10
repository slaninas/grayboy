#include "catch2/catch.hpp"
#include "cpu.h"
#include "test_utils.h"

TEST_CASE("LD BC, d16 - 0x01", "[ld]")
{
	const auto orig_memory = MemoryChanger{
	  {{0x00, 0x01},
	   {0x01, 0xEE},
	   {0x02, 0xFF},
	   {0x03, 0x01},
	   {0x04, 0xAB},
	   {0x05, 0xCD},
	   {0x06, 0x01},
	   {0x07, 0x01},
	   {0x08, 0x00}}}.get(getRandomMemory());

	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	SECTION("Copy 0xEEFF into BC")
	{
		const auto cycles = cpu.execute_next();
		CHECK(cycles == 3);

		const auto correct_registers = RegistersChanger{.BC = 0xFFEE, .PC = 0x03}.get(orig_regs);
		CHECK_THAT(cpu.registers().dump(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}

	SECTION("Copy 0xABCD into BC")
	{
		cpu.registers().write("PC", 0x03);
		const auto cycles = cpu.execute_next();
		CHECK(cycles == 3);

		const auto correct_registers = RegistersChanger{.BC = 0xCDAB, .PC = 0x06}.get(orig_regs);
		CHECK_THAT(cpu.registers().dump(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}

	SECTION("Copy 0x0100 into BC")
	{
		cpu.registers().write("PC", 0x06);
		const auto cycles = cpu.execute_next();
		CHECK(cycles == 3);

		const auto correct_registers = RegistersChanger{.BC = 0x0001, .PC = 0x09}.get(orig_regs);
		CHECK_THAT(cpu.registers().dump(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}
}

TEST_CASE("LD DE, d16 - 0x11", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x11}, {0x01, 0xab}, {0x02, 0xcd}}}.get(getRandomMemory());

	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 3);

	const auto correct_registers = RegistersChanger{.DE = 0xcdab, .PC = 0x03}.get(orig_regs);
	CHECK_THAT(cpu.registers().dump(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD HL, d16 - 0x21", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x21}, {0x01, 0x12}, {0x02, 0x34}}}.get(getRandomMemory());

	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 3);

	const auto correct_registers = RegistersChanger{.HL = 0x3412, .PC = 0x03}.get(orig_regs);
	CHECK_THAT(cpu.registers().dump(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD SP, d16 - 0x31", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x31}, {0x01, 0xdf}, {0x02, 0xea}}}.get(getRandomMemory());

	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 3);

	const auto correct_registers = RegistersChanger{.PC = 0x03, .SP = 0xeadf}.get(orig_regs);
	CHECK_THAT(cpu.registers().dump(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD (BC), A - 0x02", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x02}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	SECTION("Copy A (0x12) from A into address held in BC (0x00)")
	{
		cpu.registers().write("A", 0x12);
		cpu.registers().write("BC", 0x00);

		const auto cycles = cpu.execute_next();
		CHECK(cycles == 2);

		const auto correct_memory = MemoryChanger{{{0x00, 0x12}}}.get(orig_memory);
		CHECK(cpu.memory_dump() == correct_memory.dump());

		const auto correct_registers = RegistersChanger{.A = 0x12, .BC = 0x00, .PC = 0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}

	SECTION("Copy A (0xBC) into address held in BC (0x04)")
	{
		cpu.registers().write("A", 0xBC);
		cpu.registers().write("BC", 0x04);

		const auto cycles = cpu.execute_next();
		CHECK(cycles == 2);

		const auto correct_memory = MemoryChanger{{{0x04, 0xBC}}}.get(orig_memory);
		CHECK(cpu.memory_dump() == correct_memory.dump());

		const auto correct_registers = RegistersChanger{.A = 0xBC, .BC = 0x04, .PC = 0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}
}

TEST_CASE("LD (DE), A - 0x12", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x12}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x12, .DE = 0xde, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_memory = MemoryChanger{{{0xde, 0x12}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());

	const auto correct_registers = RegistersChanger{.PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
}

TEST_CASE("LD (HL+), A - 0x22", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x22}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xdf, .HL = 0xabcd, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_memory = MemoryChanger{{{0xabcd, 0xdf}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());

	const auto correct_registers = RegistersChanger{.HL = 0xabce, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
}

TEST_CASE("LD (HL-), A - 0x32", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x32}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xdf, .HL = 0xabcd, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_memory = MemoryChanger{{{0xabcd, 0xdf}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());

	const auto correct_registers = RegistersChanger{.HL = 0xabcc, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
}

TEST_CASE("LD B, d8 - 0x06", "[ld]")
{
	const auto orig_memory =
	  MemoryChanger{{{0x00, 0x06}, {0x01, 0xbc}, {0x02, 0x06}, {0x03, 0x12}, {0x04, 0x06}, {0x05, 0x00}}}.get(
	    getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	auto correct_regs = RegistersChanger{.B = 0xbc, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 2);
	correct_regs = RegistersChanger{.B = 0x12, .PC = 0x04}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 2);
	correct_regs = RegistersChanger{.B = 0x00, .PC = 0x06}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD D, d8 - 0x16", "[ld]")
{
	const auto orig_memory =
	  MemoryChanger{{{0x00, 0x16}, {0x01, 0xbc}, {0x02, 0x16}, {0x03, 0x12}, {0x04, 0x16}, {0x05, 0x00}}}.get(
	    getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	auto correct_regs = RegistersChanger{.D = 0xbc, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 2);
	correct_regs = RegistersChanger{.D = 0x12, .PC = 0x04}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 2);
	correct_regs = RegistersChanger{.D = 0x00, .PC = 0x06}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD H, d8 - 0x26", "[ld]")
{
	const auto orig_memory =
	  MemoryChanger{{{0x00, 0x26}, {0x01, 0xbc}, {0x02, 0x26}, {0x03, 0x12}, {0x04, 0x26}, {0x05, 0x00}}}.get(
	    getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	auto correct_regs = RegistersChanger{.H = 0xbc, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 2);
	correct_regs = RegistersChanger{.H = 0x12, .PC = 0x04}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 2);
	correct_regs = RegistersChanger{.H = 0x00, .PC = 0x06}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD (HL), d8 - 0x36", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x36}, {0x01, 0x12}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL = 0xfedc, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 3);
	const auto correct_regs = RegistersChanger{.PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0xfedc, 0x12}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("LD (a16), SP - 0x08", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x08}, {0x01, 0xfe}, {0x02, 0xc1}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00, .SP = 0xba98}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 5);
	const auto correct_regs = RegistersChanger{.PC = 0x03}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0xfec1, 0x98}, {0xfec2, 0xba}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("LD A, (BC) - 0x0a", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x0a}, {0xabcd, 0xfedc}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.BC = 0xabcd, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.A = 0xfedc, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD A, (DE) - 0x1a", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x1a}, {0x1234, 0x8734}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.DE = 0x1234, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.A = 0x8734, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD A, (HL+) - 0x2a", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x2a}, {0x40ac, 0xdfbc}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL = 0x40ac, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.A = 0xdfbc, .HL = 0x40ad, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD A, (HL-) - 0x3a", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x3a}, {0x40ac, 0xdfbc}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL = 0x40ac, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.A = 0xdfbc, .HL = 0x40ab, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD C, d8 - 0x0e", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x0e}, {0x01, 0xfe}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.C = 0xfe, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD E, d8 - 0x1e", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x1e}, {0x01, 0xdc}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.E = 0xdc, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD L, d8 - 0x2e", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x2e}, {0x01, 0x12}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.L = 0x12, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD A, d8 - 0x3e", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x3e}, {0x01, 0xca}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.A = 0xca, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD B, B - 0x40", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x40}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD B, C - 0x41", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x41}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.C = 0x45, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.B = 0x45, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD B, D - 0x42", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x42}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.D = 0xde, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.B = 0xde, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD B, E - 0x43", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x43}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.E = 0xee, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.B = 0xee, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD B, H - 0x44", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x44}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.H = 0xff, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.B = 0xff, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD B, L - 0x45", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x45}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.L = 0xed, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.B = 0xed, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD B, (HL) - 0x46", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x46}, {0xedfa, 0x34}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL = 0xedfa, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	const auto correct_regs = RegistersChanger{.B = 0x34, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD B, A - 0x47", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x47}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x00, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.B = 0x00, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD C, B - 0x48", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x48}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.B = 0x56, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.C = 0x56, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD C, C - 0x49", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x49}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD C, D - 0x4a", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x4a}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.D = 0x33, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.C = 0x33, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD C, E - 0x4b", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x4b}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.E = 0x09, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.C = 0x09, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD C, H - 0x4c", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x4c}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.H = 0x10, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.C = 0x10, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD C, L - 0x4d", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x4d}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.L = 0x5f, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.C = 0x5f, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD C, (HL) - 0x4e", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x4e}, {0x4321, 0xce}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL = 0x4321, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	const auto correct_regs = RegistersChanger{.C = 0xce, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD C, A - 0x4f", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x4f}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x8f, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.C = 0x8f, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

// This 0x5* opcodes tests are copy-pasted from previous 0x4* line, same for 0x6*
TEST_CASE("LD D, B - 0x50", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x50}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.B = 0x32, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.D = 0x32, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD D, C - 0x51", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x51}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.C = 0x45, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.D = 0x45, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD D, D - 0x52", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x52}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD D, E - 0x53", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x53}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.E = 0xee, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.D = 0xee, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD D, H - 0x54", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x54}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.H = 0xff, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.D = 0xff, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD D, L - 0x55", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x55}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.L = 0xed, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.D = 0xed, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD D, (HL) - 0x56", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x56}, {0xedfa, 0x34}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL = 0xedfa, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	const auto correct_regs = RegistersChanger{.D = 0x34, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD D, A - 0x57", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x57}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x00, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.D = 0x00, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD E, B - 0x58", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x58}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.B = 0x56, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.E = 0x56, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD E, C - 0x59", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x59}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.C = 0xfe, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.E = 0xfe, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD E, D - 0x5a", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x5a}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.D = 0x33, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.E = 0x33, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD E, E - 0x5b", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x5b}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD E, H - 0x5c", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x5c}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.H = 0x10, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.E = 0x10, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD E, L - 0x5d", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x5d}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.L = 0x5f, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.E = 0x5f, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD E, (HL) - 0x5e", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x5e}, {0x4321, 0xce}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL = 0x4321, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	const auto correct_regs = RegistersChanger{.E = 0xce, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD E, A - 0x5f", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x5f}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x8f, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.E = 0x8f, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

// ----------------------------- 0x6* -----------------------------

TEST_CASE("LD H, B - 0x60", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x60}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.B = 0x32, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.H = 0x32, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD H, C - 0x61", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x61}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.C = 0x45, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.H = 0x45, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD H, D - 0x62", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x62}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.D = 0x45, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.H = 0x45, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD H, E - 0x63", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x63}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.E = 0xee, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.H = 0xee, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD H, H - 0x64", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x64}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD H, L - 0x65", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x65}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.L = 0xed, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.H = 0xed, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD H, (HL) - 0x66", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x66}, {0xedfa, 0x34}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL = 0xedfa, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	const auto correct_regs = RegistersChanger{.H = 0x34, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD H, A - 0x67", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x67}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x00, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.H = 0x00, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD L, B - 0x68", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x68}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.B = 0x56, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.L = 0x56, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD L, C - 0x69", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x69}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.C = 0xfe, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.L = 0xfe, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD L, D - 0x6a", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x6a}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.D = 0x33, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.L = 0x33, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD L, E - 0x6b", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x6b}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.E = 0xbc, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.L = 0xbc, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD L, H - 0x6c", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x6c}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.H = 0x10, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.L = 0x10, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD L, L - 0x6d", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x6d}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD L, (HL) - 0x6e", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x6e}, {0x4321, 0xce}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL = 0x4321, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	const auto correct_regs = RegistersChanger{.L = 0xce, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD L, A - 0x6f", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x6f}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x8f, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.L = 0x8f, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

// ----------------------------- 0x7* minus 0x76 -----------------------------

TEST_CASE("LD (HL), B - 0x70", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x70}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.B = 0x8f, .HL = 0xbcad, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0xbcad, 0x8f}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("LD (HL), C - 0x71", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x71}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.C = 0xa9, .HL = 0xabcd, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0xabcd, 0xa9}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("LD (HL), D - 0x72", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x72}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.D = 0x78, .HL = 0x00ab, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0x00ab, 0x78}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("LD (HL), E - 0x73", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x73}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.E = 0xef, .HL = 0xab00, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0xab00, 0xef}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

// TODO: Does 0x74 and 0x75 handle HL correctly? Shoudln't be H and L swapped?
TEST_CASE("LD (HL), H - 0x74", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x74}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL = 0xabcd, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0xabcd, 0xab}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("LD (HL), L - 0x75", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x75}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL = 0x1234, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0x1234, 0x34}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("LD (HL), A - 0x77", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x77}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xb3, .HL = 0xbccb, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0xbccb, 0xb3}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("LD A, B - 0x78", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x78}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.B = 0x1a, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_regs = RegistersChanger{.A = 0x1a, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD A, C - 0x79", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x79}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.C = 0x3c, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_regs = RegistersChanger{.A = 0x3c, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD A, D - 0x7a", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x7a}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.D = 0x59, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_regs = RegistersChanger{.A = 0x59, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD A, E - 0x7b", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x7b}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.E = 0x59, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_regs = RegistersChanger{.A = 0x59, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD A, H - 0x7c", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x7c}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.H = 0x0a, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_regs = RegistersChanger{.A = 0x0a, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD A, L - 0x7d", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x7d}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.L = 0xac, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);

	const auto correct_regs = RegistersChanger{.A = 0xac, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD A, (HL) - 0x7e", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x7e}, {0x12bc, 0x9a}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL = 0x12bc, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.A = 0x9a, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD A, A - 0x7f", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0x7f}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	const auto correct_regs = RegistersChanger{.PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD (a8), A - 0xe0", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0xe0}, {0x01, 0x34}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xab, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 3);
	const auto correct_regs = RegistersChanger{.PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0xff34, 0xab}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("LD A, (a8) - 0xf0", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0xf0}, {0x01, 0xde}, {0xffde, 0x89}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 3);
	const auto correct_regs = RegistersChanger{.A = 0x89, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD (C), A - 0xe2", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0xe2}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0x45, .C = 0xae, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	const auto correct_regs = RegistersChanger{.PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0xffae, 0x45}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("LD A, (C) - 0xf2", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0xf2}, {0xff87, 0x90}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.C = 0x87, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	const auto correct_regs = RegistersChanger{.A = 0x90, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("POP BC - 0xc1", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0xc1}, {0x0bd1, 0x43}, {0x0bd2, 0xbd}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00, .SP = 0x0bd1}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 3);
	const auto correct_regs = RegistersChanger{.BC = 0xbd43, .PC = 0x01, .SP = 0x0bd3}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("POP DE - 0xd1", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0xd1}, {0xf001, 0xde}, {0xf002, 0x89}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00, .SP = 0xf001}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 3);
	const auto correct_regs = RegistersChanger{.DE = 0x89de, .PC = 0x01, .SP = 0xf003}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("POP HL - 0xe1", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0xe1}, {0xde12, 0xab}, {0xde13, 0x12}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00, .SP = 0xde12}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 3);
	const auto correct_regs = RegistersChanger{.HL = 0x12ab, .PC = 0x01, .SP = 0xde14}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("POP AF - 0xf1", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0xf1}, {0x8912, 0xff}, {0x8913, 0xee}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00, .SP = 0x8912}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 3);
	const auto correct_regs = RegistersChanger{.AF = 0xeeff, .PC = 0x01, .SP = 0x8914}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("PUSH BC - 0xc5", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0xc5}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.BC = 0xbcde, .PC = 0x00, .SP = 0x3278}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 4);
	const auto correct_regs = RegistersChanger{.PC = 0x01, .SP = 0x3276}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0x3277, 0xbc}, {0x3276, 0xde}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("PUSH DE - 0xd5", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0xd5}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.DE = 0xbcde, .PC = 0x00, .SP = 0x3278}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 4);
	const auto correct_regs = RegistersChanger{.PC = 0x01, .SP = 0x3276}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0x3277, 0xbc}, {0x3276, 0xde}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("PUSH HL - 0xe5", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0xe5}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL = 0xbcde, .PC = 0x00, .SP = 0x3278}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 4);
	const auto correct_regs = RegistersChanger{.PC = 0x01, .SP = 0x3276}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0x3277, 0xbc}, {0x3276, 0xde}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("PUSH AF - 0xf5", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0xf5}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.AF = 0xbcde, .PC = 0x00, .SP = 0x3278}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 4);
	const auto correct_regs = RegistersChanger{.PC = 0x01, .SP = 0x3276}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0x3277, 0xbc}, {0x3276, 0xde}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("LD HL, SP+s8 - 0xf8", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0xf8}, {0x01, 0x23}}}.get(getRandomMemory());
	const auto orig_flags = getRandomFlags();
	const auto orig_regs =
	  RegistersChanger{.F = orig_flags, .HL = 0x1000, .PC = 0x00, .SP = 0x2000}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 3);
	const auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_flags);
	const auto correct_regs = RegistersChanger{.F = correct_flags, .HL = 0x3023, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD SP, HL - 0xf9", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0xf9}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL = 0xa981, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	const auto correct_regs = RegistersChanger{.PC = 0x01, .SP = 0xa981}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD (a16), A - 0xea", "[ld]")
{
	const auto orig_memory = MemoryChanger{{{0x00, 0xea}, {0x01, 0xfe}, {0x02, 0xc1}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xba, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 4);
	const auto correct_regs = RegistersChanger{.PC = 0x03}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0xfec1, 0xba}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("LD A, (a16) - 0xfa", "[ld]")
{
	const auto orig_memory =
	  MemoryChanger{{{0x00, 0xfa}, {0x01, 0xfe}, {0x02, 0xc1}, {0xc1fe, 0xbc}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 4);
	const auto correct_regs = RegistersChanger{.A = 0xbc, .PC = 0x03}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}
