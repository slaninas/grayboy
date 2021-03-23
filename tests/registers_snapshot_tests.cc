#include "catch2/catch.hpp"
#include "registers.h"
#include "test_utils.h"

TEST_CASE("MakeRegisters", "[registers]")
{
	const auto orig_registers = getRandomRegisters();
	auto registers = orig_registers;

	auto registers_snapshot = RegistersSnaphost{registers};

	CHECK(registers_snapshot.get(0) == orig_registers);

	SECTION("One back")
	{
		registers.write("AF", 0x6543);
		registers_snapshot.add(registers);

		CHECK(registers_snapshot.get(1) == orig_registers);
	}

	SECTION("One back, more diffs at once")
	{
		registers.write("SP", 0x1234);
		registers.write("PC", 0xbdec);
		registers_snapshot.add(registers);

		CHECK(registers_snapshot.get(1) == orig_registers);
	}

	SECTION("Empty snapshots")
	{
		registers.write("SP", 0x6543);
		registers_snapshot.add(registers);
		registers_snapshot.add(registers);
		registers_snapshot.add(registers);
		registers_snapshot.add(registers);
		CHECK(registers_snapshot.get(4) == orig_registers);
	}

	SECTION("More snapshots")
	{
		registers.write("A", 0x12);
		registers_snapshot.add(registers);
		registers.write("B", 0xbc);
		registers_snapshot.add(registers);
		registers.write("C", 0xde);
		registers_snapshot.add(registers);

		CHECK(registers_snapshot.get(1) == RegistersChanger{.A = 0x12, .B = 0xbc}.get(orig_registers));
		CHECK(registers_snapshot.get(2) == RegistersChanger{.A = 0x12}.get(orig_registers));
		CHECK(registers_snapshot.get(3) == orig_registers);
	}
}
