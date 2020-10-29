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

		const auto correct_registers = RegistersChanger{.BC=0xFFEE, .PC=0x03}.get(orig_regs);
		CHECK_THAT(cpu.registers_dump(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}

	SECTION("Copy 0xABCD into BC") {
		cpu.registers().write("PC", 0x03);
		const auto cycles = cpu.execute_next();
		CHECK(cycles == 3);

		const auto correct_registers = RegistersChanger{.BC=0xCDAB, .PC=0x06}.get(orig_regs);
		CHECK_THAT(cpu.registers_dump(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}

	SECTION("Copy 0x0100 into BC") {
		cpu.registers().write("PC", 0x06);
		const auto cycles = cpu.execute_next();
		CHECK(cycles == 3);

		const auto correct_registers = RegistersChanger{.BC=0x0001, .PC=0x09}.get(orig_regs);
		CHECK_THAT(cpu.registers_dump(), RegistersCompare{correct_registers});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}
}

TEST_CASE("LD DE, d16 - 0x11", "[ld]") {
	const auto orig_memory = MemoryChanger{{
		{0x00, 0x11}, {0x01, 0xab}, {0x02, 0xcd}
	}}.get(getRandomMemory());

	const auto orig_regs = RegistersChanger{.PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 3);

	const auto correct_registers = RegistersChanger{.DE=0xcdab, .PC=0x03}.get(orig_regs);
	CHECK_THAT(cpu.registers_dump(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD HL, d16 - 0x21", "[ld]") {
	const auto orig_memory = MemoryChanger{{
		{0x00, 0x21}, {0x01, 0x12}, {0x02, 0x34}
	}}.get(getRandomMemory());

	const auto orig_regs = RegistersChanger{.PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 3);

	const auto correct_registers = RegistersChanger{.HL=0x3412, .PC=0x03}.get(orig_regs);
	CHECK_THAT(cpu.registers_dump(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD SP, d16 - 0x31", "[ld]") {
	const auto orig_memory = MemoryChanger{{
		{0x00, 0x31}, {0x01, 0xdf}, {0x02, 0xea}
	}}.get(getRandomMemory());

	const auto orig_regs = RegistersChanger{.PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 3);

	const auto correct_registers = RegistersChanger{.PC=0x03, .SP=0xeadf}.get(orig_regs);
	CHECK_THAT(cpu.registers_dump(), RegistersCompare{correct_registers});
	CHECK(cpu.memory_dump() == orig_memory.dump());
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

TEST_CASE("LD (DE), A - 0x12", "[ld]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x12}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A=0x12, .DE=0xde, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_memory = MemoryChanger{{{0xde, 0x12}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());

	const auto correct_registers = RegistersChanger{.PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
}

TEST_CASE("LD (HL+), A - 0x22", "[ld]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x22}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A=0xdf, .HL=0xabcd, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_memory = MemoryChanger{{{0xabcd, 0xdf}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());

	const auto correct_registers = RegistersChanger{.HL=0xabce, .PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
}

TEST_CASE("LD (HL-), A - 0x32", "[ld]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x32}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A=0xdf, .HL=0xabcd, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_memory = MemoryChanger{{{0xabcd, 0xdf}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());

	const auto correct_registers = RegistersChanger{.HL=0xabcc, .PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
}

TEST_CASE("LD B, d8 - 0x06", "[ld]") {
	const auto orig_memory = MemoryChanger{{
		{0x00, 0x06}, {0x01, 0xbc},
		{0x02, 0x06}, {0x03, 0x12},
		{0x04, 0x06}, {0x05, 0x00}
	}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	auto correct_regs = RegistersChanger{.B=0xbc, .PC=0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());


	cycles = cpu.execute_next();
	CHECK(cycles == 2);
	correct_regs = RegistersChanger{.B=0x12, .PC=0x04}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 2);
	correct_regs = RegistersChanger{.B=0x00, .PC=0x06}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD D, d8 - 0x16", "[ld]") {
	const auto orig_memory = MemoryChanger{{
		{0x00, 0x16}, {0x01, 0xbc},
		{0x02, 0x16}, {0x03, 0x12},
		{0x04, 0x16}, {0x05, 0x00}
	}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	auto correct_regs = RegistersChanger{.D=0xbc, .PC=0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());


	cycles = cpu.execute_next();
	CHECK(cycles == 2);
	correct_regs = RegistersChanger{.D=0x12, .PC=0x04}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 2);
	correct_regs = RegistersChanger{.D=0x00, .PC=0x06}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD H, d8 - 0x26", "[ld]") {
	const auto orig_memory = MemoryChanger{{
		{0x00, 0x26}, {0x01, 0xbc},
		{0x02, 0x26}, {0x03, 0x12},
		{0x04, 0x26}, {0x05, 0x00}
	}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	auto correct_regs = RegistersChanger{.H=0xbc, .PC=0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());


	cycles = cpu.execute_next();
	CHECK(cycles == 2);
	correct_regs = RegistersChanger{.H=0x12, .PC=0x04}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());

	cycles = cpu.execute_next();
	CHECK(cycles == 2);
	correct_regs = RegistersChanger{.H=0x00, .PC=0x06}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD (HL), d8 - 0x36", "[ld]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x36}, {0x01, 0x12}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL=0xfedc, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 3);
	const auto correct_regs = RegistersChanger{.PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0xfedc, 0x12}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("LD (a16), SP - 0x08", "[ld]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x08}, {0x01, 0xfe}, {0x02, 0xc1}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC=0x00, .SP=0xba98}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 5);
	const auto correct_regs = RegistersChanger{.PC=0x03}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0xfec1, 0x98}, {0xfec2, 0xba}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("LD A, (BC) - 0x0a", "[ld]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x0a}, {0xabcd, 0xfedc}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.BC=0xabcd, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.A=0xfedc, .PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD A, (DE) - 0x1a", "[ld]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x1a}, {0x1234, 0x8734}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.DE=0x1234, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.A=0x8734, .PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD A, (HL+) - 0x2a", "[ld]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x2a}, {0x40ac, 0xdfbc}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL=0x40ac, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.A=0xdfbc, .HL=0x40ad, .PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD A, (HL-) - 0x3a", "[ld]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x3a}, {0x40ac, 0xdfbc}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.HL=0x40ac, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.A=0xdfbc, .HL=0x40ab, .PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD C, d8 - 0x0e", "[ld]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x0e}, {0x01, 0xfe}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.C=0xfe, .PC=0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD E, d8 - 0x1e", "[ld]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x1e}, {0x01, 0xdc}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.E=0xdc, .PC=0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD L, d8 - 0x2e", "[ld]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x2e}, {0x01, 0x12}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.L=0x12, .PC=0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("LD A, d8 - 0x3e", "[ld]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x3e}, {0x01, 0xca}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	const auto cycles = cpu.execute_next();
	CHECK(cycles == 2);

	const auto correct_regs = RegistersChanger{.A=0xca, .PC=0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	CHECK(cpu.memory_dump() == orig_memory.dump());
}
