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
