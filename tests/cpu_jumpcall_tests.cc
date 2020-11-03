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
		const auto orig_flags = FlagsChanger{.Z=0}.get(getRandomFlags());
		const auto orig_regs = RegistersChanger{.F=orig_flags, .PC=0x00, .SP=0x1234}.get(getRandomRegisters());
		auto cpu = Cpu{orig_memory, orig_regs};

		auto cycles = cpu.execute_next();
		CHECK(cycles == 5);
		auto correct_regs = RegistersChanger{.PC=0xaecb, .SP=0x1236}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}

	SECTION("Do not return") {
		const auto orig_flags = FlagsChanger{.Z=1}.get(getRandomFlags());
		const auto orig_regs = RegistersChanger{.F=orig_flags, .PC=0x00, .SP=0x1234}.get(getRandomRegisters());
		auto cpu = Cpu{orig_memory, orig_regs};

		auto cycles = cpu.execute_next();
		CHECK(cycles == 2);
		auto correct_regs = RegistersChanger{.PC=0x01}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}
}

TEST_CASE("JP NZ, a16 - 0xc2", "[jump/call]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xc2}, {0x01, 0x12}, {0x02, 0x34}}}.get(getRandomMemory());

	SECTION("Do jump") {
		const auto orig_flags = FlagsChanger{.Z=0}.get(getRandomFlags());
		const auto orig_regs = RegistersChanger{.F=orig_flags, .PC=0x00}.get(getRandomRegisters());
		auto cpu = Cpu{orig_memory, orig_regs};

		auto cycles = cpu.execute_next();
		CHECK(cycles == 4);
		auto correct_regs = RegistersChanger{.PC=0x3412}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}

	SECTION("Do not jump") {
		const auto orig_flags = FlagsChanger{.Z=1}.get(getRandomFlags());
		const auto orig_regs = RegistersChanger{.F=orig_flags, .PC=0x00}.get(getRandomRegisters());
		auto cpu = Cpu{orig_memory, orig_regs};

		auto cycles = cpu.execute_next();
		CHECK(cycles == 3);
		auto correct_regs = RegistersChanger{.PC=0x03}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}
}

TEST_CASE("JP NC, a16 - 0xd2", "[jump/call]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xd2}, {0x01, 0xab}, {0x02, 0xcd}}}.get(getRandomMemory());

	SECTION("Do jump") {
		const auto orig_flags = FlagsChanger{.C=0}.get(getRandomFlags());
		const auto orig_regs = RegistersChanger{.F=orig_flags, .PC=0x00}.get(getRandomRegisters());
		auto cpu = Cpu{orig_memory, orig_regs};

		auto cycles = cpu.execute_next();
		CHECK(cycles == 4);
		auto correct_regs = RegistersChanger{.PC=0xcdab}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}

	SECTION("Do not jump") {
		const auto orig_flags = FlagsChanger{.C=1}.get(getRandomFlags());
		const auto orig_regs = RegistersChanger{.F=orig_flags, .PC=0x00}.get(getRandomRegisters());
		auto cpu = Cpu{orig_memory, orig_regs};

		auto cycles = cpu.execute_next();
		CHECK(cycles == 3);
		auto correct_regs = RegistersChanger{.PC=0x03}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}
}

TEST_CASE("JP a16 - 0xc3", "[jump/call]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xc3}, {0x01, 0x22}, {0x02, 0x43}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 4);
	auto correct_regs = RegistersChanger{.PC=0x4322}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	CHECK(cpu.memory_dump() == orig_memory.dump());
}

TEST_CASE("CALL NZ, a16 - 0xc4", "[jump/call]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xc4}, {0x01, 0x27}, {0x02, 0x90}}}.get(getRandomMemory());

	SECTION("Do call") {
		const auto orig_flags = FlagsChanger{.Z=0}.get(getRandomFlags());
		const auto orig_regs = RegistersChanger{.F=orig_flags, .PC=0x00, .SP=0x4358}.get(getRandomRegisters());
		auto cpu = Cpu{orig_memory, orig_regs};

		auto cycles = cpu.execute_next();
		CHECK(cycles == 6);
		auto correct_regs = RegistersChanger{.PC=0x9027, .SP=0x4356}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
		const auto correct_memory = MemoryChanger{{{0x4357, 0x00}, {0x4356, 0x03}}}.get(orig_memory);
		CHECK(cpu.memory_dump() == correct_memory.dump());
	}

	SECTION("Do not call") {
		const auto orig_flags = FlagsChanger{.Z=1}.get(getRandomFlags());
		const auto orig_regs = RegistersChanger{.F=orig_flags, .PC=0x00}.get(getRandomRegisters());
		auto cpu = Cpu{orig_memory, orig_regs};

		auto cycles = cpu.execute_next();
		CHECK(cycles == 3);
		auto correct_regs = RegistersChanger{.PC=0x03}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}
}

TEST_CASE("CALL NC, a16 - 0xd4", "[jump/call]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0xd4}, {0x01, 0x27}, {0x02, 0x90}}}.get(getRandomMemory());

	SECTION("Do call") {
		const auto orig_flags = FlagsChanger{.C=0}.get(getRandomFlags());
		const auto orig_regs = RegistersChanger{.F=orig_flags, .PC=0x00, .SP=0x4358}.get(getRandomRegisters());
		auto cpu = Cpu{orig_memory, orig_regs};

		auto cycles = cpu.execute_next();
		CHECK(cycles == 6);
		auto correct_regs = RegistersChanger{.PC=0x9027, .SP=0x4356}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
		const auto correct_memory = MemoryChanger{{{0x4357, 0x00}, {0x4356, 0x03}}}.get(orig_memory);
		CHECK(cpu.memory_dump() == correct_memory.dump());
	}

	SECTION("Do not call") {
		const auto orig_flags = FlagsChanger{.C=1}.get(getRandomFlags());
		const auto orig_regs = RegistersChanger{.F=orig_flags, .PC=0x00}.get(getRandomRegisters());
		auto cpu = Cpu{orig_memory, orig_regs};

		auto cycles = cpu.execute_next();
		CHECK(cycles == 3);
		auto correct_regs = RegistersChanger{.PC=0x03}.get(orig_regs);
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
		CHECK(cpu.memory_dump() == orig_memory.dump());
	}
}

TEST_CASE("RST 0 - 0xc7", "[jump/call]") {
	const auto orig_memory = MemoryChanger{{{0x7845, 0xc7}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC=0x7845, .SP=0xabcd}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 4);
	auto correct_regs = RegistersChanger{.PC=0x00, .SP=0xabcb}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0xabcc, 0x78}, {0xabcb, 0x45}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("RST 2 - 0xd7", "[jump/call]") {
	const auto orig_memory = MemoryChanger{{{0x7845, 0xd7}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC=0x7845, .SP=0xabcd}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 4);
	auto correct_regs = RegistersChanger{.PC=0x10, .SP=0xabcb}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0xabcc, 0x78}, {0xabcb, 0x45}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("RST 4 - 0xe7", "[jump/call]") {
	const auto orig_memory = MemoryChanger{{{0x7845, 0xe7}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC=0x7845, .SP=0xabcd}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 4);
	auto correct_regs = RegistersChanger{.PC=0x20, .SP=0xabcb}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0xabcc, 0x78}, {0xabcb, 0x45}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("RST 6 - 0xf7", "[jump/call]") {
	const auto orig_memory = MemoryChanger{{{0x7845, 0xf7}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC=0x7845, .SP=0xabcd}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 4);
	auto correct_regs = RegistersChanger{.PC=0x30, .SP=0xabcb}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0xabcc, 0x78}, {0xabcb, 0x45}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("RST 1 - 0xcf", "[jump/call]") {
	const auto orig_memory = MemoryChanger{{{0x7845, 0xcf}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC=0x7845, .SP=0xabcd}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 4);
	auto correct_regs = RegistersChanger{.PC=0x08, .SP=0xabcb}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0xabcc, 0x78}, {0xabcb, 0x45}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("RST 3 - 0xdf", "[jump/call]") {
	const auto orig_memory = MemoryChanger{{{0x7845, 0xdf}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC=0x7845, .SP=0xabcd}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 4);
	auto correct_regs = RegistersChanger{.PC=0x18, .SP=0xabcb}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0xabcc, 0x78}, {0xabcb, 0x45}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("RST 5 - 0xef", "[jump/call]") {
	const auto orig_memory = MemoryChanger{{{0x7845, 0xef}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC=0x7845, .SP=0xabcd}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 4);
	auto correct_regs = RegistersChanger{.PC=0x28, .SP=0xabcb}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0xabcc, 0x78}, {0xabcb, 0x45}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}

TEST_CASE("RST 7 - 0xff", "[jump/call]") {
	const auto orig_memory = MemoryChanger{{{0x7845, 0xff}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.PC=0x7845, .SP=0xabcd}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 4);
	auto correct_regs = RegistersChanger{.PC=0x38, .SP=0xabcb}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
	const auto correct_memory = MemoryChanger{{{0xabcc, 0x78}, {0xabcb, 0x45}}}.get(orig_memory);
	CHECK(cpu.memory_dump() == correct_memory.dump());
}
