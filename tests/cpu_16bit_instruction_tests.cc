#include "catch2/catch.hpp"

#include "test_utils.h"

#include "cpu.h"

// NOTE: Instructions with 8bit opcodes were tested individually, here I will test just functions for groups of instructions
// TODO: Add more tests?
TEST_CASE("RLC", "[bit_operations]") {
	const auto orig_flags = FlagsChanger{.C=0}.get(getRandomFlags());
	const auto orig_regs = RegistersChanger{.F=orig_flags, .B=0xf4}.get(getRandomRegisters());

	auto regs = orig_regs;
	instruction_rlc("B", regs);

	const auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=0, .C=1}.get(orig_flags);
	const auto correct_regs = RegistersChanger{.F=correct_flags, .B=0xe9}.get(orig_regs);
	CHECK_THAT(regs, RegistersCompare{correct_regs});
}

TEST_CASE("RL", "[bit_operations]") {
	const auto orig_flags = FlagsChanger{.C=0}.get(getRandomFlags());
	const auto orig_regs = RegistersChanger{.F=orig_flags, .B=0xf4}.get(getRandomRegisters());

	auto regs = orig_regs;
	instruction_rl("B", regs);

	const auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=0, .C=1}.get(orig_flags);
	const auto correct_regs = RegistersChanger{.F=correct_flags, .B=0xe8}.get(orig_regs);
	CHECK_THAT(regs, RegistersCompare{correct_regs});
}

TEST_CASE("RRC", "[bit_operations]") {
	const auto orig_flags = FlagsChanger{.C=0}.get(getRandomFlags());
	const auto orig_regs = RegistersChanger{.F=orig_flags, .B=0xe9}.get(getRandomRegisters());

	auto regs = orig_regs;
	instruction_rrc("B", regs);

	const auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=0, .C=1}.get(orig_flags);
	const auto correct_regs = RegistersChanger{.F=correct_flags, .B=0xf4}.get(orig_regs);
	CHECK_THAT(regs, RegistersCompare{correct_regs});
}

TEST_CASE("RR", "[bit_operations]") {
	const auto orig_flags = FlagsChanger{.C=0}.get(getRandomFlags());
	const auto orig_regs = RegistersChanger{.F=orig_flags, .B=0xe9}.get(getRandomRegisters());

	auto regs = orig_regs;
	instruction_rr("B", regs);

	const auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=0, .C=1}.get(orig_flags);
	const auto correct_regs = RegistersChanger{.F=correct_flags, .B=0x74}.get(orig_regs);
	CHECK_THAT(regs, RegistersCompare{correct_regs});
}

TEST_CASE("SLA", "[bit_operations]") {
	const auto orig_flags = FlagsChanger{.C=0}.get(getRandomFlags());
	const auto orig_regs = RegistersChanger{.F=orig_flags, .B=0xe9}.get(getRandomRegisters());

	auto regs = orig_regs;
	instruction_sla("B", regs);

	const auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=0, .C=1}.get(orig_flags);
	const auto correct_regs = RegistersChanger{.F=correct_flags, .B=0xd2}.get(orig_regs);
	CHECK_THAT(regs, RegistersCompare{correct_regs});
}

TEST_CASE("SRA", "[bit_operations]") {
	const auto orig_flags = FlagsChanger{.C=0}.get(getRandomFlags());
	const auto orig_regs = RegistersChanger{.F=orig_flags, .B=0xe9}.get(getRandomRegisters());

	auto regs = orig_regs;
	instruction_sra("B", regs);

	const auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=0, .C=1}.get(orig_flags);
	const auto correct_regs = RegistersChanger{.F=correct_flags, .B=0xf4}.get(orig_regs);
	CHECK_THAT(regs, RegistersCompare{correct_regs});
}

TEST_CASE("SWAP", "[bit_operations]") {
	const auto orig_flags = getRandomFlags();
	const auto orig_regs = RegistersChanger{.F=orig_flags, .B=0xe9}.get(getRandomRegisters());

	auto regs = orig_regs;
	instruction_swap("B", regs);

	const auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=0, .C=0}.get(orig_flags);
	const auto correct_regs = RegistersChanger{.F=correct_flags, .B=0x9e}.get(orig_regs);
	CHECK_THAT(regs, RegistersCompare{correct_regs});
}

TEST_CASE("SRL", "[bit_operations]") {
	const auto orig_flags = getRandomFlags();
	const auto orig_regs = RegistersChanger{.F=orig_flags, .B=0xe9}.get(getRandomRegisters());

	auto regs = orig_regs;
	instruction_srl("B", regs);

	const auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=0, .C=1}.get(orig_flags);
	const auto correct_regs = RegistersChanger{.F=correct_flags, .B=0x74}.get(orig_regs);
	CHECK_THAT(regs, RegistersCompare{correct_regs});
}

TEST_CASE("BIT", "[bit_operations]") {
	const auto orig_flags = getRandomFlags();
	const auto orig_regs = RegistersChanger{.F=orig_flags, .B=0xd6}.get(getRandomRegisters());
	auto regs = orig_regs;

	SECTION("Bit #0") {
		instruction_bit("B", 0, regs);
		const auto correct_flags = FlagsChanger{.Z=1, .N=0, .H=1}.get(orig_flags);
		const auto correct_regs = RegistersChanger{.F=correct_flags}.get(orig_regs);
		CHECK_THAT(regs, RegistersCompare{correct_regs});
	}

	SECTION("Bit #1") {
		instruction_bit("B", 1, regs);
		const auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=1}.get(orig_flags);
		const auto correct_regs = RegistersChanger{.F=correct_flags}.get(orig_regs);
		CHECK_THAT(regs, RegistersCompare{correct_regs});
	}

	SECTION("Bit #2") {
		instruction_bit("B", 2, regs);
		const auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=1}.get(orig_flags);
		const auto correct_regs = RegistersChanger{.F=correct_flags}.get(orig_regs);
		CHECK_THAT(regs, RegistersCompare{correct_regs});
	}

	SECTION("Bit #3") {
		instruction_bit("B", 3, regs);
		const auto correct_flags = FlagsChanger{.Z=1, .N=0, .H=1}.get(orig_flags);
		const auto correct_regs = RegistersChanger{.F=correct_flags}.get(orig_regs);
		CHECK_THAT(regs, RegistersCompare{correct_regs});
	}

	SECTION("Bit #4") {
		instruction_bit("B", 4, regs);
		const auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=1}.get(orig_flags);
		const auto correct_regs = RegistersChanger{.F=correct_flags}.get(orig_regs);
		CHECK_THAT(regs, RegistersCompare{correct_regs});
	}

	SECTION("Bit #5") {
		instruction_bit("B", 5, regs);
		const auto correct_flags = FlagsChanger{.Z=1, .N=0, .H=1}.get(orig_flags);
		const auto correct_regs = RegistersChanger{.F=correct_flags}.get(orig_regs);
		CHECK_THAT(regs, RegistersCompare{correct_regs});
	}

	SECTION("Bit #6") {
		instruction_bit("B", 6, regs);
		const auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=1}.get(orig_flags);
		const auto correct_regs = RegistersChanger{.F=correct_flags}.get(orig_regs);
		CHECK_THAT(regs, RegistersCompare{correct_regs});
	}

	SECTION("Bit #7") {
		instruction_bit("B", 7, regs);
		const auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=1}.get(orig_flags);
		const auto correct_regs = RegistersChanger{.F=correct_flags}.get(orig_regs);
		CHECK_THAT(regs, RegistersCompare{correct_regs});
	}
}
