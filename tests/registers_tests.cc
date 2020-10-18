#include "catch2/catch.hpp"

#include "test_utils.h"

#include "registers.h"

// TODO: Add more (copy/move ctor/assignment) for Registers class
// TODO: Add test for MakeRegisters, test even the incorrect use (e.g. MakeRegisters{.AF=0x0000, .A=0x11}

TEST_CASE("Registers initialization", "[registers]") {
	SECTION("Default initialization") {
		auto regs = Registers{};
		CHECK(regs.dump() == Registers::ArrayType{});
	}
	SECTION("Initialization with non-zeroed array") {
		// Fill array with something
		const auto reg_array = Registers::ArrayType{0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xde, 0xf0, 0xaa, 0xbb, 0xcc};
		auto regs = Registers{reg_array};
		CHECK(regs.dump() == reg_array);
	}
}

TEST_CASE("Registers read", "[registers]") {
		// Fill array with something
	const auto reg_array = Registers::ArrayType{0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xde, 0xf0, 0xaa, 0xbb, 0xcc};
	auto regs = Registers{reg_array};

	CHECK(regs.read("AF") == 0x2301);
	CHECK(regs.read("F") == 0x01);
	CHECK(regs.read("A") == 0x23);

	CHECK(regs.read("BC") == 0x6745);
	CHECK(regs.read("C") == 0x45);
	CHECK(regs.read("B") == 0x67);

	CHECK(regs.read("DE") == 0xab89);
	CHECK(regs.read("E") == 0x89);
	CHECK(regs.read("D") == 0xab);

	CHECK(regs.read("HL") == 0xdecd);
	CHECK(regs.read("L") == 0xcd);
	CHECK(regs.read("H") == 0xde);

	CHECK(regs.read("PC") == 0xaaf0);
	CHECK(regs.read("SP") == 0xccbb);
}

TEST_CASE("Registers write", "[registers]") {
		// Fill array with something
	auto regs = MakeRegisters{.AF=0x2301, .BC=0x6745, .DE=0xab89, .HL=0xdecd, .PC=0xaaf0, .SP=0xccbb}.get();


	// NOTE: Do not test it this way, how it's internally isn't important but it's important what read should produce same value as write wrote
	// SECTION("Write AF") {
		// regs.write("AF", 0x0123);
		// auto correct_reg_array = reg_array;
		// correct_reg_array[0] = 0x23;
		// correct_reg_array[1] = 0x01;
		// CHECK(regs.dump() == correct_reg_array);
	// }
	SECTION("AF") {
		SECTION("AF") {
			regs.write("AF", 0xabcd);
			const auto correct_regs = MakeRegisters{.AF=0xabcd, .BC=0x6745, .DE=0xab89, .HL=0xdecd, .PC=0xaaf0, .SP=0xccbb}.get();
			CHECK_THAT(regs, RegistersCompare{correct_regs});
		}
		SECTION("A") {
			regs.write("A", 0xab);
			const auto correct_regs = MakeRegisters{.AF=0xab01, .BC=0x6745, .DE=0xab89, .HL=0xdecd, .PC=0xaaf0, .SP=0xccbb}.get();
			CHECK_THAT(regs, RegistersCompare{correct_regs});
		}
		SECTION("F") {
			regs.write("F", 0xab);
			const auto correct_regs = MakeRegisters{.AF=0x23ab, .BC=0x6745, .DE=0xab89, .HL=0xdecd, .PC=0xaaf0, .SP=0xccbb}.get();
			CHECK_THAT(regs, RegistersCompare{correct_regs});
		}
	}

	SECTION("BC") {
		SECTION("BC") {
			regs.write("BC", 0xbcde);
			const auto correct_regs = MakeRegisters{.AF=0x2301, .BC=0xbcde, .DE=0xab89, .HL=0xdecd, .PC=0xaaf0, .SP=0xccbb}.get();
			CHECK_THAT(regs, RegistersCompare{correct_regs});
		}
		SECTION("B") {
			regs.write("B", 0xbe);
			const auto correct_regs = MakeRegisters{.AF=0x2301, .BC=0xbe45, .DE=0xab89, .HL=0xdecd, .PC=0xaaf0, .SP=0xccbb}.get();
			CHECK_THAT(regs, RegistersCompare{correct_regs});
		}
		SECTION("C") {
			regs.write("C", 0xce);
			const auto correct_regs = MakeRegisters{.AF=0x2301, .BC=0x67ce, .DE=0xab89, .HL=0xdecd, .PC=0xaaf0, .SP=0xccbb}.get();
			CHECK_THAT(regs, RegistersCompare{correct_regs});
		}
	}

	SECTION("DE") {
		SECTION("DE") {
			regs.write("DE", 0xdefa);
			const auto correct_regs = MakeRegisters{.AF=0x2301, .BC=0x6745, .DE=0xdefa, .HL=0xdecd, .PC=0xaaf0, .SP=0xccbb}.get();
			CHECK_THAT(regs, RegistersCompare{correct_regs});
		}
		SECTION("D") {
			regs.write("D", 0xde);
			const auto correct_regs = MakeRegisters{.AF=0x2301, .BC=0x6745, .DE=0xde89, .HL=0xdecd, .PC=0xaaf0, .SP=0xccbb}.get();
			CHECK_THAT(regs, RegistersCompare{correct_regs});
		}
		SECTION("E") {
			regs.write("E", 0xef);
			const auto correct_regs = MakeRegisters{.AF=0x2301, .BC=0x6745, .DE=0xabef, .HL=0xdecd, .PC=0xaaf0, .SP=0xccbb}.get();
			CHECK_THAT(regs, RegistersCompare{correct_regs});
		}
	}

	SECTION("HL") {
		SECTION("HL") {
			regs.write("HL", 0xfedc);
			const auto correct_regs = MakeRegisters{.AF=0x2301, .BC=0x6745, .DE=0xab89, .HL=0xfedc, .PC=0xaaf0, .SP=0xccbb}.get();
			CHECK_THAT(regs, RegistersCompare{correct_regs});
		}
		SECTION("H") {
			regs.write("H", 0xfd);
			const auto correct_regs = MakeRegisters{.AF=0x2301, .BC=0x6745, .DE=0xab89, .HL=0xfdcd, .PC=0xaaf0, .SP=0xccbb}.get();
			CHECK_THAT(regs, RegistersCompare{correct_regs});
		}
		SECTION("L") {
			regs.write("L", 0xab);
			const auto correct_regs = MakeRegisters{.AF=0x2301, .BC=0x6745, .DE=0xab89, .HL=0xdeab, .PC=0xaaf0, .SP=0xccbb}.get();
			CHECK_THAT(regs, RegistersCompare{correct_regs});
		}
	}

	SECTION("PC") {
		regs.write("PC", 0xfbcd);
		const auto correct_regs = MakeRegisters{.AF=0x2301, .BC=0x6745, .DE=0xab89, .HL=0xdecd, .PC=0xfbcd, .SP=0xccbb}.get();
		CHECK_THAT(regs, RegistersCompare{correct_regs});
	}

	SECTION("SP") {
		regs.write("SP", 0xbfde);
		const auto correct_regs = MakeRegisters{.AF=0x2301, .BC=0x6745, .DE=0xab89, .HL=0xdecd, .PC=0xaaf0, .SP=0xbfde}.get();
		CHECK_THAT(regs, RegistersCompare{correct_regs});
	}
}


