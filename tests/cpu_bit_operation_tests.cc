
#include "catch2/catch.hpp"

#include "test_utils.h"
#include "cpu.h"

TEST_CASE("RLCA 0 0x07", "[bit_operations]") {
	const auto orig_memory = MemoryChanger{{{0x00, 0x07}, {0x01, 0x07}, {0x02, 0x07}}}.get(getRandomMemory());
	const auto orig_regs = RegistersChanger{.A=0xbf, .PC=0x00}.get(getRandomRegisters());
	auto cpu = Cpu{orig_memory, orig_regs};

	auto cycles = cpu.execute_next();
	CHECK(cycles == 1);
	auto correct_flags = FlagsChanger{.Z=0, .N=0, .H=0, .C=1}.get(orig_regs.read("F"));
	auto correct_regs = RegistersChanger{.A=0x7f, .F=correct_flags, .PC=0x01}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	cycles = cpu.execute_next();
	CHECK(cycles == 1);

	correct_flags = FlagsChanger{.Z=0, .N=0, .H=0, .C=0}.get(correct_flags);
	correct_regs = RegistersChanger{.A=0xfe, .F=correct_flags, .PC=0x02}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});

	cycles = cpu.execute_next();
	CHECK(cycles == 1);
	correct_flags = FlagsChanger{.Z=0, .N=0, .H=0, .C=1}.get(correct_flags);
	correct_regs = RegistersChanger{.A=0xfd, .F=correct_flags, .PC=0x03}.get(orig_regs);
	CHECK_THAT(cpu.registers(), RegistersCompare{correct_regs});
}
