#pragma once

#include <iostream>
#include <stdexcept>
#include <cassert>

// TODO: Zero registers or do not initialize at all or like original ROM?
// TODO: Add unit tests for registers
// TODO: Use assert and run in debug instead of throwing when it makes sense
class Registers{
public:
	static const size_t ArrayElements = 12;
	using ArrayType = std::array<uint8_t, ArrayElements>;

	Registers() = default;
	// TODO: Remove this constructor, use MakeRegisters only
	Registers(const ArrayType& regs_array) : register_array_{regs_array} {}

	void clear() {
		std::fill(begin(register_array_), end(register_array_), 0x0);
	}

	template<size_t kSize>
	[[nodiscard]] auto read(const char(&reg_name)[kSize]) const {
		constexpr auto reg_name_size = kSize - 1; // Subtract \0 at the end
		const auto reg_index = register_index(reg_name);

		// 8bit registers
		if constexpr(reg_name_size == 1) {
			return register_array_[reg_index];;
		}
		// Combined 16bit registers
		else {
			return *reinterpret_cast<const uint16_t*>(register_array_.data() + reg_index);
		}
	}


	template<size_t kSize>
	void write(const char(&reg_name)[kSize], uint16_t value) {
		constexpr auto reg_name_size = kSize - 1; // Subtract \0 at the end
		const auto reg_index = register_index(reg_name);

		// 8bit registers
		if constexpr(reg_name_size == 1) {
			assert(((value & static_cast<uint16_t>(0xff00)) == 0) && "Writing 16bit value into 8bit register is not allowed.");
			register_array_[reg_index] = value;
		}
		// Combined 16bit registers
		else {
			*reinterpret_cast<uint16_t*>(register_array_.data() + reg_index) = value;
		}
	}

	template<size_t kSize>
	[[nodiscard]] auto read_flag(const char(&flag_name)[kSize]) const {
		// TODO: Use static_assert
		assert((kSize == 2) && "Flags are only one letter (+ \n), you cannot address them by more letters.");
		const auto flag = std::string_view{flag_name};
		if (flag == "Z") return static_cast<bool>(read("F") & (1 << 7));
		else if (flag == "N") return static_cast<bool>(read("F") & (1 << 6));
		else if (flag == "H") return static_cast<bool>(read("F") & (1 << 5));
		else if (flag == "C") return static_cast<bool>(read("F") & (1 << 4));

		assert(false && "You should not be here, it means you called read_flag with incorrect flag name.");
	}

	template<size_t kSize>
	void set_flag(const char(&flag_name)[kSize], const bool value) {
		// TODO: Use static_assert
		assert((kSize == 2) && "Flags are only one letter (+ \n), you cannot address them by more letters.");
		const char flag = flag_name[0];
		const auto F_value = read("F");

		switch (flag) {
			case 'Z':
				write("F", value ? F_value | (1 << 7) : F_value ^ (1 << 7));
				break;
			case 'N':
				write("F", value ? F_value | (1 << 6) : F_value ^ (1 << 6));
				break;
			case 'H':
				write("F", value ? F_value | (1 << 5) : F_value ^ (1 << 5));
				break;
			case 'C':
				write("F", value ? F_value | (1 << 4) : F_value ^ (1 << 4));
				break;

			default:
				assert(false && "You should not be here, it means you called set_flag with incorrect flag name.");
				break;
		}
	}

	auto& print(std::ostream& os) const {
		os << std::hex;
		auto print_pair = [&](const auto& name, const auto& both, const auto& hi, const auto& lo) {
			os << name << ": " << static_cast<int>(both);
			os << "\t[" << name[0] << "=" << static_cast<int>(hi) << ' ' << name[1] << "="<< static_cast<int>(lo) << ']';
			os << '\n';
		};

		os << "Registers: \n" << std::string(20, '-') << '\n';
		// print_pair("AF", read("AF"), read("A"), read("F"));
		print_pair("AF", read("AF"), read("A"), read("F"));
		print_pair("BC", read("BC"), read("B"), read("C"));
		print_pair("DE", read("DE"), read("D"), read("E"));
		print_pair("HL", read("HL"), read("H"), read("L"));

		os << "PC: " << static_cast<int>(read("PC")) << '\n';
		os << "SP: " << static_cast<int>(read("SP")) << '\n';
		os << "Flags: ";
		os << "Z=" << read_flag("Z") << " N=" << read_flag("N") << " H=" << read_flag("H") << " C=" << read_flag("C") << "]\n";

		os << std::dec;
		return os;
	}

	void print() const {
		print(std::cout);
	}

	auto dump() const {
		return register_array_;
	}

private:
	std::array<uint8_t, 12> register_array_ = {};

	int register_index(const char* reg_name) const {
		const auto reg= std::string_view{reg_name};

		if (reg == "AF") return 0;
		else if (reg == "F") return 0;
		else if (reg == "A") return 1;

		else if (reg == "BC") return 2;
		else if (reg == "C") return 2;
		else if (reg == "B") return 3;

		else if (reg == "DE") return 4;
		else if (reg == "E") return 4;
		else if (reg == "D") return 5;

		else if (reg == "HL") return 6;
		else if (reg == "L") return 6;
		else if (reg == "H") return 7;

		else if (reg == "PC") return 8;
		else if (reg == "SP") return 10;
		else throw std::logic_error(std::string("Register ") + reg_name + " doesn't exist");
	}

};

std::ostream& operator<<(std::ostream& os, const Registers& registers) {
	return registers.print(os);
}

struct MakeRegisters{
	std::optional<uint16_t> AF;
	std::optional<uint8_t> A;
	std::optional<uint8_t> F;
	std::optional<uint16_t> BC;
	std::optional<uint8_t> B;
	std::optional<uint8_t> C;
	std::optional<uint16_t> DE;
	std::optional<uint8_t> D;
	std::optional<uint8_t> E;
	std::optional<uint16_t> HL;
	std::optional<uint8_t> H;
	std::optional<uint8_t> L;
	std::optional<uint16_t> PC;
	std::optional<uint16_t> SP;


	auto get() {
		check_consistency();

		const auto A_val = static_cast<uint8_t>(AF.has_value() ? (AF.value() & 0xFF00) >> 8 : A.value_or(0x00));
		const auto F_val = static_cast<uint8_t>(AF.has_value() ? AF.value() & 0x00FF : F.value_or(0x00));

		const auto B_val = static_cast<uint8_t>(BC.has_value() ? (BC.value() & 0xFF00) >> 8 : B.value_or(0x00));
		const auto C_val = static_cast<uint8_t>(BC.has_value() ? BC.value() & 0x00FF : C.value_or(0x00));

		const auto D_val = static_cast<uint8_t>(DE.has_value() ? (DE.value() & 0xFF00) >> 8 : D.value_or(0x00));
		const auto E_val = static_cast<uint8_t>(DE.has_value() ? DE.value() & 0x00FF : E.value_or(0x00));

		const auto H_val = static_cast<uint8_t>(HL.has_value() ? (HL.value() & 0xFF00) >> 8 : H.value_or(0x00));
		const auto L_val = static_cast<uint8_t>(HL.has_value() ? HL.value() & 0x00FF : L.value_or(0x00));

		const auto PC_val = static_cast<uint16_t>(PC.value_or(0x0000));
		const auto SP_val = static_cast<uint16_t>(SP.value_or(0x0000));

		auto array = std::array<uint8_t, 12>{F_val, A_val, C_val, B_val, E_val, D_val, L_val, H_val};
		auto registers = Registers{array};
		registers.write("PC", PC_val);
		registers.write("SP", SP_val);
		return registers;
	}

	void check_consistency() {
		assert(!(AF.has_value() && (A.has_value() || F.has_value())) && "You can't set AF and A (or F) at the same time");
		assert(!(BC.has_value() && (B.has_value() || C.has_value())) && "You can't set BC and B (or C) at the same time");
		assert(!(DE.has_value() && (D.has_value() || E.has_value())) && "You can't set AF and D (or E) at the same time");
		assert(!(HL.has_value() && (H.has_value() || L.has_value())) && "You can't set HL and H (or L) at the same time");
	}

};

/// TODO: Add something like RegistersChanger that will use existing registers + will change given registers,
//        const auto old_regs = MakeRegisters{...}.get();
//        const auto changed_regs = ChangeRegisters{.A=0x00}.get(old_regs);
//        Do the same for flags

struct MakeFlags {
	std::optional<bool> Z;
	std::optional<bool> N;
	std::optional<bool> H;
	std::optional<bool> C;
	std::optional<uint8_t> unused;

	[[nodiscard]] auto get() {
		auto value = static_cast<uint8_t>(0x00);
		value += static_cast<uint8_t>(Z.value_or(0x00)) << 7;
		value += static_cast<uint8_t>(N.value_or(0x00)) << 6;
		value += static_cast<uint8_t>(H.value_or(0x00)) << 5;
		value += static_cast<uint8_t>(C.value_or(0x00)) << 4;
		value += static_cast<uint8_t>(unused.value_or(0x00)) << 0;
		return value;
	}
};

struct FlagsChanger {
	std::optional<bool> Z;
	std::optional<bool> N;
	std::optional<bool> H;
	std::optional<bool> C;
	std::optional<uint8_t> unused;

	[[nodiscard]] auto get(const uint8_t orig_flags) {
		auto Z_val = Z.value_or(static_cast<bool>(orig_flags & (1 << 7)));
		auto N_val = N.value_or(static_cast<bool>(orig_flags & (1 << 6)));
		auto H_val = H.value_or(static_cast<bool>(orig_flags & (1 << 5)));
		auto C_val = C.value_or(static_cast<bool>(orig_flags & (1 << 4)));
		auto unused_val = unused.value_or(static_cast<uint8_t>(orig_flags & 0x0F));

		return MakeFlags{.Z=Z_val, .N=N_val, .H=H_val, .C=C_val, .unused=unused_val}.get();
	}
};
