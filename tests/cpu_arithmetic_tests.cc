#include "catch2/catch.hpp"

#include "test_utils.h"

#include "cpu.h"

// TODO: Fuzzy testing where it makes sense, even in other test files

TEST_CASE("INC BC - 0x03", "[arithmetic]") {
	// TODO: Check flags - they should stay the same for INC BC
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
	auto cpu = Cpu{Memory{{0x04, 0x04, 0x04}}};
	const auto F_init = 0xff;
	const auto F_low_nibble = F_init & 0x0f;
	cpu.registers().write("F", F_init);

	SECTION("From zero to three") {
		auto cycles = cpu.execute_next();
		CHECK(cycles == 1);
		auto correct_F = MakeFlags{.Z=0, .N=0, .H=0, .C=1}.get() + F_low_nibble;
		auto correct_registers = MakeRegisters{.F=correct_F, .B=0x01, .PC=0x01}.get();
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_F = MakeFlags{.Z=0, .N=0, .H=0, .C=1}.get() + F_low_nibble;
		correct_registers = MakeRegisters{.F=correct_F, .B=0x02, .PC=0x02}.get();
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_F = MakeFlags{.Z=0, .N=0, .H=0, .C=1}.get() + F_low_nibble;
		correct_registers = MakeRegisters{.F=correct_F, .B=0x03, .PC=0x03}.get();
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}

	SECTION("Z flag") {
		cpu.registers().write("B", 0xff);
		auto cycles = cpu.execute_next();
		CHECK(cycles == 1);

		const auto correct_F = MakeFlags{.Z=1, .N=0, .H=1, .C=1}.get() + F_low_nibble;
		const auto correct_registers = MakeRegisters{.F=correct_F, .B=0x00, .PC=0x01}.get();
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}

	SECTION("H flag") {
		cpu.registers().write("B", 0x0f);
		auto cycles = cpu.execute_next();
		CHECK(cycles == 1);

		auto correct_F = MakeFlags{.Z=0, .N=0, .H=1, .C=1}.get() + F_low_nibble;
		auto correct_registers = MakeRegisters{.F=correct_F, .B=0x10, .PC=0x01}.get();
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_F = MakeFlags{.Z=0, .N=0, .H=0, .C=1}.get() + F_low_nibble;
		correct_registers = MakeRegisters{.F=correct_F, .B=0x11, .PC=0x02}.get();
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}

}

TEST_CASE("DEC B - 0x05", "[arithmetic]") {
	auto cpu = Cpu{Memory{{0x05, 0x05, 0x05}}};
	const auto F_init = 0xff;
	const auto F_low_nibble = F_init & 0x0f;
	cpu.registers().write("F", F_init);

	SECTION("Three to zero") {
		cpu.registers().write("B", 0x03);
		auto cycles = cpu.execute_next();
		CHECK(cycles == 1);

		// C is unchanged, it's 1 just because it was 1 before
		auto correct_F = MakeFlags{.Z=0, .N=1, .H=0, .C=1}.get() + F_low_nibble;
		auto correct_registers = MakeRegisters{.F=correct_F, .B=0x02, .PC=0x01}.get();
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_F = MakeFlags{.Z=0, .N=1, .H=0, .C=1}.get() + F_low_nibble;
		correct_registers = MakeRegisters{.F=correct_F, .B=0x01, .PC=0x02}.get();
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_F = MakeFlags{.Z=1, .N=1, .H=0, .C=1}.get() + F_low_nibble;
		correct_registers = MakeRegisters{.F=correct_F, .B=0x00, .PC=0x03}.get();
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}

	// TODO: More half-carry tests
	SECTION("Half-carry") {
		cpu.registers().write("B", 0xf0);
		const auto cycles = cpu.execute_next();
		CHECK(cycles == 1);

		const auto correct_F = MakeFlags{.Z=0, .N=1, .H=1, .C=1}.get() + F_low_nibble;
		const auto correct_registers = MakeRegisters{.F=correct_F, .B=0xef, .PC=0x01}.get();
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}

}

// TODO: Start using FlagsChanger
