#include "catch2/catch.hpp"

#include "test_utils.h"

#include "registers.h"

// TODO: Add more (copy/move ctor/assignment) for Registers class
// TODO: Add test for MakeRegisters, test even the incorrect use (e.g. MakeRegisters{.AF=0x0000, .A=0x11}

TEST_CASE("MakeRegisters", "[registers]") {

	SECTION("One register set only") {

		const auto regs = MakeRegisters{.F=0xa}.get();

		CHECK(regs.read("A") == 0x0);
		CHECK(regs.read("F") == 0xa);
		CHECK(regs.read("BC") == 0x00);
		CHECK(regs.read("DE") == 0x00);
		CHECK(regs.read("HL") == 0x00);
		CHECK(regs.read("PC") == 0x00);
		CHECK(regs.read("SP") == 0x00);

	}

	SECTION("More registers set at once") {
		const auto regs = MakeRegisters{.AF=0xab, .BC=0xcd, .DE=0xef, .HL=0x12, .PC=0x34, .SP=0x56}.get();
		CHECK(regs.read("AF") == 0xab);
		CHECK(regs.read("BC") == 0xcd);
		CHECK(regs.read("DE") == 0xef);
		CHECK(regs.read("HL") == 0x12);
		CHECK(regs.read("PC") == 0x34);
		CHECK(regs.read("SP") == 0x56);
	}
}

TEST_CASE("RegistersChanger", "[registers]") {
	const auto orig_regs = getRandomRegisters();
	auto changed_regs = RegistersChanger{}.get(orig_regs);
	CHECK_THAT(changed_regs.dump(), RegistersCompare{orig_regs});

	changed_regs = RegistersChanger{.D=0xd}.get(orig_regs);
	CHECK(changed_regs.read("AF") == orig_regs.read("AF"));
	CHECK(changed_regs.read("BC") == orig_regs.read("BC"));
	CHECK(changed_regs.read("D") == 0xd);
	CHECK(changed_regs.read("E") == orig_regs.read("E"));
	CHECK(changed_regs.read("HL") == orig_regs.read("HL"));
	CHECK(changed_regs.read("PC") == orig_regs.read("PC"));
	CHECK(changed_regs.read("SP") == orig_regs.read("SP"));

	changed_regs = RegistersChanger{.AF=0xcd, .BC=0xfe, .DE=0x12, .HL=0x56, .PC=0x87, .SP=0x43}.get(orig_regs);
	CHECK(changed_regs.read("AF") == 0xcd);
	CHECK(changed_regs.read("BC") == 0xfe);
	CHECK(changed_regs.read("DE") == 0x12);
	CHECK(changed_regs.read("HL") == 0x56);
	CHECK(changed_regs.read("PC") == 0x87);
	CHECK(changed_regs.read("SP") == 0x43);

}

TEST_CASE("MakeFlags", "[registers]") {
	SECTION("One flag set only") {
		auto flags = MakeFlags{}.get();
		CHECK(flags == 0x00);

		flags = MakeFlags{.Z=1}.get();
		CHECK(flags == 0x80);

		flags = MakeFlags{.N=1}.get();
		CHECK(flags == 0x40);

		flags = MakeFlags{.H=1}.get();
		CHECK(flags == 0x20);

		flags = MakeFlags{.C=1}.get();
		CHECK(flags == 0x10);
	}

	SECTION("More flags set at once") {
		auto flags = MakeFlags{.Z=1, .N=1, .H=0, .C=0, .unused=0xf}.get();
		CHECK(flags == 0xcf);

		flags = MakeFlags{.Z=1, .N=1, .H=1, .C=1, .unused=0xe}.get();
		CHECK(flags == 0xfe);

		flags = MakeFlags{.Z=0, .N=1, .H=0, .C=1, .unused=0xd}.get();
		CHECK(flags == 0x5d);

		// Same but using default 0 for some flags
		flags = MakeFlags{.N=1, .C=1, .unused=0xd}.get();
		CHECK(flags == 0x5d);
	}
}

TEST_CASE("FlagsChanger", "[registers]") {
	SECTION("Change one flag at the time") {
		const auto orig_flags = MakeFlags{}.get();

		auto changed_flags = FlagsChanger{.Z=1}.get(orig_flags);
		CHECK(changed_flags == 0x80);

		changed_flags = FlagsChanger{.N=1}.get(orig_flags);
		CHECK(changed_flags == 0x40);

		changed_flags = FlagsChanger{.H=1}.get(orig_flags);
		CHECK(changed_flags == 0x20);

		changed_flags = FlagsChanger{.C=1}.get(orig_flags);
		CHECK(changed_flags == 0x10);

		changed_flags = FlagsChanger{.unused=0xf}.get(orig_flags);
		CHECK(changed_flags == 0x0f);
	}

	SECTION("More flags changed at once") {
		SECTION("From all unset flags") {
			const auto orig_flags = MakeFlags{}.get();
			auto changed_flags = FlagsChanger{.Z=1, .N=1, .H=1, .C=1, .unused=0xf}.get(orig_flags);
			CHECK(changed_flags == 0xff);

			changed_flags = FlagsChanger{.Z=1, .N=0, .H=0, .C=1, .unused=0xe}.get(orig_flags);
			CHECK(changed_flags == 0x9e);

			changed_flags = FlagsChanger{.Z=1}.get(orig_flags);
			CHECK(changed_flags == 0x80);

			changed_flags = FlagsChanger{.unused=0xf}.get(orig_flags);
			CHECK(changed_flags == 0x0f);
		}

		SECTION("From all set flags") {
			const auto orig_flags = static_cast<uint8_t>(0xff);
			auto changed_flags = FlagsChanger{.Z=0, .N=0, .H=0, .C=0, .unused=0xf}.get(orig_flags);
			CHECK(changed_flags == 0x0f);

			changed_flags = FlagsChanger{.Z=1, .N=0, .H=0, .C=1, .unused=0xe}.get(orig_flags);
			CHECK(changed_flags == 0x9e);

			changed_flags = FlagsChanger{.Z=0}.get(orig_flags);
			CHECK(changed_flags == 0x7f);

			changed_flags = FlagsChanger{.unused=0xa}.get(orig_flags);
			CHECK(changed_flags == 0xfa);
		}
	}

}

TEST_CASE("Registers initialization", "[registers]") {
	SECTION("Default initialization") {
		auto regs = Registers{};
		CHECK(regs.dump() == Registers::ArrayType{});
	}
	SECTION("Initialization with non-zeroed array, then clear") {
		// Fill array with something
		const auto reg_array = Registers::ArrayType{0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xde, 0xf0, 0xaa, 0xbb, 0xcc};
		auto regs = Registers{reg_array};
		CHECK(regs.dump() == reg_array);

		regs.clear();
		CHECK(regs.dump() == Registers::ArrayType{});
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


TEST_CASE("Registers' flags set/read", "[registers]") {
	const auto F_init = 0x0f;
	const auto F_lower_nibbel = F_init & 0x0f;
	auto regs = MakeRegisters{.F=F_init}.get();

	CHECK(regs.read_flag("Z") == false);
	CHECK(regs.read_flag("N") == false);
	CHECK(regs.read_flag("H") == false);
	CHECK(regs.read_flag("C") == false);
	CHECK((regs.read("F") & 0x0f) == F_lower_nibbel);

	SECTION("Z") {
		regs.set_flag("Z", false);
		CHECK(regs.read_flag("Z") == true);
		CHECK(regs.read_flag("N") == false);
		CHECK(regs.read_flag("H") == false);
		CHECK(regs.read_flag("C") == false);
		CHECK((regs.read("F") & 0x0f) == F_lower_nibbel);
	}
	SECTION("N") {
		regs.set_flag("N", false);
		CHECK(regs.read_flag("Z") == false);
		CHECK(regs.read_flag("N") == true);
		CHECK(regs.read_flag("H") == false);
		CHECK(regs.read_flag("C") == false);
		CHECK((regs.read("F") & 0x0f) == F_lower_nibbel);
	}
	SECTION("H") {
		regs.set_flag("H", false);
		CHECK(regs.read_flag("Z") == false);
		CHECK(regs.read_flag("N") == false);
		CHECK(regs.read_flag("H") == true);
		CHECK(regs.read_flag("C") == false);
		CHECK((regs.read("F") & 0x0f) == F_lower_nibbel);
	}
	SECTION("C") {
		regs.set_flag("C", false);
		CHECK(regs.read_flag("Z") == false);
		CHECK(regs.read_flag("N") == false);
		CHECK(regs.read_flag("H") == false);
		CHECK(regs.read_flag("C") == true);
		CHECK((regs.read("F") & 0x0f) == F_lower_nibbel);
	}
}

TEST_CASE("Registers' flags unset/read", "[registers]") {
	const auto F_init = 0xf0;
	const auto F_lower_nibbel = F_init & 0x0f;
	auto regs = MakeRegisters{.F=F_init}.get();

	CHECK(regs.read_flag("Z") == true);
	CHECK(regs.read_flag("N") == true);
	CHECK(regs.read_flag("H") == true);
	CHECK(regs.read_flag("C") == true);
	CHECK((regs.read("F") & 0x0f) == F_lower_nibbel);

	SECTION("Z") {
		regs.set_flag("Z", false);
		CHECK(regs.read_flag("Z") == false);
		CHECK(regs.read_flag("N") == true);
		CHECK(regs.read_flag("H") == true);
		CHECK(regs.read_flag("C") == true);
		CHECK((regs.read("F") & 0x0f) == F_lower_nibbel);
	}
	SECTION("N") {
		regs.set_flag("N", false);
		CHECK(regs.read_flag("Z") == true);
		CHECK(regs.read_flag("N") == false);
		CHECK(regs.read_flag("H") == true);
		CHECK(regs.read_flag("C") == true);
		CHECK((regs.read("F") & 0x0f) == F_lower_nibbel);
	}
	SECTION("H") {
		regs.set_flag("H", false);
		CHECK(regs.read_flag("Z") == true);
		CHECK(regs.read_flag("N") == true);
		CHECK(regs.read_flag("H") == false);
		CHECK(regs.read_flag("C") == true);
		CHECK((regs.read("F") & 0x0f) == F_lower_nibbel);
	}
	SECTION("C") {
		regs.set_flag("C", false);
		CHECK(regs.read_flag("Z") == true);
		CHECK(regs.read_flag("N") == true);
		CHECK(regs.read_flag("H") == true);
		CHECK(regs.read_flag("C") == false);
		CHECK((regs.read("F") & 0x0f) == F_lower_nibbel);
	}
}


