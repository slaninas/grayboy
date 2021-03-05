
#include "catch2/catch.hpp"
#include "cpu.h"
#include "test_utils.h"

TEST_CASE("RLCA - 0x07", "[bit_operations]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x07}, {0x01, 0x07}, {0x02, 0x07}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xbf, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 1}.get(orig_regs.read("F"));
	auto correct_regs = RegistersChanger{.A = 0x7f, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	cycles = cpu.execute_next();
	CHECK(cycles == 1);

	correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(correct_flags);
	correct_regs = RegistersChanger{.A = 0xfe, .F = correct_flags, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	cycles = cpu.execute_next();
	CHECK(cycles == 1);
	correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 1}.get(correct_flags);
	correct_regs = RegistersChanger{.A = 0xfd, .F = correct_flags, .PC = 0x03}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
}

TEST_CASE("RLA - 0x17", "[bit_operations]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x17}, {0x01, 0x17}, {0x02, 0x17}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C = 1}.get(getRandomFlags());
	const auto orig_regs = RegistersChanger{.A = 0xbf, .F = orig_flags, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 1}.get(orig_regs.read("F"));
	auto correct_regs = RegistersChanger{.A = 0x7f, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	cycles = cpu.execute_next();
	CHECK(cycles == 1);

	correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(correct_flags);
	correct_regs = RegistersChanger{.A = 0xff, .F = correct_flags, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	cycles = cpu.execute_next();
	CHECK(cycles == 1);
	correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 1}.get(correct_flags);
	correct_regs = RegistersChanger{.A = 0xfe, .F = correct_flags, .PC = 0x03}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
}

TEST_CASE("RRCA - 0x0f", "[bit_operations]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x0f}, {0x01, 0x0f}, {0x02, 0x0f}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A = 0xFD, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 1}.get(orig_regs.read("F"));
	auto correct_regs = RegistersChanger{.A = 0xfe, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	cycles = cpu.execute_next();
	CHECK(cycles == 1);

	correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(correct_flags);
	correct_regs = RegistersChanger{.A = 0x7f, .F = correct_flags, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	cycles = cpu.execute_next();
	CHECK(cycles == 1);
	correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 1}.get(correct_flags);
	correct_regs = RegistersChanger{.A = 0xbf, .F = correct_flags, .PC = 0x03}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
}

TEST_CASE("RRA - 0x1f", "[bit_operations]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x1f}, {0x01, 0x1f}, {0x02, 0x1f}}}.get(getRandomMemory());
	const auto orig_flags = FlagsChanger{.C = 0}.get(getRandomFlags());
	const auto orig_regs = RegistersChanger{.A = 0xFD, .F = orig_flags, .PC = 0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	auto correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 1}.get(orig_flags);
	auto correct_regs = RegistersChanger{.A = 0x7e, .F = correct_flags, .PC = 0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	cycles = cpu.execute_next();
	CHECK(cycles == 1);

	correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 0}.get(orig_flags);
	correct_regs = RegistersChanger{.A = 0xbf, .F = correct_flags, .PC = 0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	cycles = cpu.execute_next();
	CHECK(cycles == 1);
	correct_flags = FlagsChanger{.Z = 0, .N = 0, .H = 0, .C = 1}.get(orig_flags);
	correct_regs = RegistersChanger{.A = 0x5f, .F = correct_flags, .PC = 0x03}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
}
