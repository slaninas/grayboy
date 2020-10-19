#include "catch2/catch.hpp"

#include "test_utils.h"

#include "cpu.h"

// TODO: Fuzzy testing where it makes sense, even in other test files
// TODO: sed 's/nibbel/nibble/g' ?

TEST_CASE("INC BC - 0x03", "[arithmetic]") {
	// TODO: Check flags - they should stay the same for INC BC
	// Increment BC three times from zero
	auto cpu = Cpu{{0x03, 0x03, 0x03}};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 2);
	auto correct_registers = MakeRegisters{.BC=0x01, .PC=0x01}.get();
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

	cycles = cpu.execute_next();
	CHECK(cycles == 2);
	correct_registers = MakeRegisters{.BC=0x02, .PC=0x02}.get();
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

	cycles = cpu.execute_next();
	CHECK(cycles == 2);
	correct_registers = MakeRegisters{.BC=0x03, .PC=0x03}.get();
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
}

TEST_CASE("INC B - 0x04", "[arithmetic]") {
	auto cpu = Cpu{{0x04, 0x04, 0x04}};
	const auto F_init = 0xff;
	const auto F_low_nibbel = F_init & 0x0f;
	cpu.registers().write("F", F_init);

	SECTION("From zero to three") {
		auto cycles = cpu.execute_next();
		CHECK(cycles == 1);
		auto correct_F = MakeFlags{.Z=0, .N=0, .H=0, .C=1}.get() + F_low_nibbel;
		auto correct_registers = MakeRegisters{.F=correct_F, .B=0x01, .PC=0x01}.get();
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_F = MakeFlags{.Z=0, .N=0, .H=0, .C=1}.get() + F_low_nibbel;
		correct_registers = MakeRegisters{.F=correct_F, .B=0x02, .PC=0x02}.get();
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_F = MakeFlags{.Z=0, .N=0, .H=0, .C=1}.get() + F_low_nibbel;
		correct_registers = MakeRegisters{.F=correct_F, .B=0x03, .PC=0x03}.get();
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}

	SECTION("Z flag") {
		cpu.registers().write("B", 0xff);
		auto cycles = cpu.execute_next();
		CHECK(cycles == 1);

		const auto correct_F = MakeFlags{.Z=1, .N=0, .H=1, .C=1}.get() + F_low_nibbel;
		const auto correct_registers = MakeRegisters{.F=correct_F, .B=0x00, .PC=0x01}.get();
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}

	SECTION("H flag") {
		cpu.registers().write("B", 0x0f);
		auto cycles = cpu.execute_next();
		CHECK(cycles == 1);

		auto correct_F = MakeFlags{.Z=0, .N=0, .H=1, .C=1}.get() + F_low_nibbel;
		auto correct_registers = MakeRegisters{.F=correct_F, .B=0x10, .PC=0x01}.get();
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});

		cycles = cpu.execute_next();
		CHECK(cycles == 1);
		correct_F = MakeFlags{.Z=0, .N=0, .H=0, .C=1}.get() + F_low_nibbel;
		correct_registers = MakeRegisters{.F=correct_F, .B=0x11, .PC=0x02}.get();
		CHECK_THAT(cpu.registers(), RegistersCompare{correct_registers});
	}

}
