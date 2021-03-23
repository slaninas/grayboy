#pragma once

#include <cassert>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

// TODO: Zero registers or do not initialize at all or like original ROM?
// TODO: Add unit tests for registers
class Registers {
public:
	static const size_t ArrayElementCount = 12;
	using ArrayType = std::array<uint8_t, ArrayElementCount>;

	Registers() = default;
	// TODO: Remove this constructor, use MakeRegisters only
	Registers(const ArrayType& regs_array) : register_array_{regs_array} {}

	void clear()
	{
		std::fill(begin(register_array_), end(register_array_), 0x0);
	}

	template<size_t kSize>
	[[nodiscard]] auto read(const char (&reg_name)[kSize]) const
	{
		constexpr auto reg_name_size = kSize - 1; // Subtract \0 at the end
		const auto reg_index = register_index(reg_name);

		// 8bit registers
		if constexpr (reg_name_size == 1) {
			return register_array_[reg_index];
		}
		// Combined 16bit registers
		else {
			return *reinterpret_cast<const uint16_t*>(register_array_.data() + reg_index);
		}
	}

	// TODO: Hide this and set_flag methos, use one method instead of write + set_flag
	template<size_t kSize>
	void write(const char (&reg_name)[kSize], uint16_t value)
	{
		constexpr auto reg_name_size = kSize - 1; // Subtract \0 at the end
		const auto reg_index = register_index(reg_name);

		// 8bit registers
		if constexpr (reg_name_size == 1) {
			assert(
			  ((value & static_cast<uint16_t>(0xff00)) == 0) &&
			  "Writing 16bit value into 8bit register is not allowed.");
			register_array_[reg_index] = value;
		}
		// Combined 16bit registers
		else {
			*reinterpret_cast<uint16_t*>(register_array_.data() + reg_index) = value;
		}
	}

	template<size_t kSize>
	[[nodiscard]] auto read_flag(const char (&flag_name)[kSize]) const
	{
		// TODO: Use static_assert
		assert((kSize == 2) && "Flags are only one letter (+ \n), you cannot address them by more letters.");
		const auto flag = std::string_view{flag_name};
		if (flag == "Z") { return static_cast<bool>(read("F") & (1 << 7)); }
		if (flag == "N") { return static_cast<bool>(read("F") & (1 << 6)); }
		if (flag == "H") { return static_cast<bool>(read("F") & (1 << 5)); }
		if (flag == "C") { return static_cast<bool>(read("F") & (1 << 4)); }

		assert(false && "You should not be here, it means you called read_flag with incorrect flag name.");
	}

	// TODO: Use set + reset flag methods instead?
	template<size_t kSize>
	void set_flag(const char (&flag_name)[kSize], const bool value)
	{
		// TODO: Use static_assert
		assert((kSize == 2) && "Flags are only one letter (+ \n), you cannot address them by more letters.");
		const char flag = flag_name[0];
		const auto F_value = read("F");

		switch (flag) {
			case 'Z':
				if (value) { write("F", F_value | 1 << 7); }
				else if (F_value & (1 << 7)) {
					write("F", F_value ^ (1 << 7));
				}
				break;
			case 'N':
				if (value) { write("F", F_value | 1 << 6); }
				else if (F_value & (1 << 6)) {
					write("F", F_value ^ (1 << 6));
				}
				break;
			case 'H':
				if (value) { write("F", F_value | 1 << 5); }
				else if (F_value & (1 << 5)) {
					write("F", F_value ^ (1 << 5));
				}
				break;
			case 'C':
				if (value) { write("F", F_value | 1 << 4); }
				else if (F_value & (1 << 4)) {
					write("F", F_value ^ (1 << 4));
				}
				break;

			default:
				assert(false && "You should not be here, it means you called set_flag with incorrect flag name.");
				break;
		}
	}

	void print(std::ostream& os) const
	{
		auto result = std::ostringstream{};
		result << std::hex;
		auto print_pair = [&](const auto& name, const auto& hi, const auto& lo) {
			result << name << ": ";
			result << std::setw(2) << std::setfill('0') << static_cast<int>(hi);
			result << ' ';
			result << std::setw(2) << std::setfill('0') << static_cast<int>(lo);
			result << '\n';
		};

		result << "Registers: \n" << std::string(15, '-') << '\n';

		print_pair("AF", read("A"), read("F"));
		print_pair("BC", read("B"), read("C"));
		print_pair("DE", read("D"), read("E"));
		print_pair("HL", read("H"), read("L"));

		result << "SP: " << std::setw(4) << std::setfill('0') << static_cast<int>(read("SP")) << '\n';
		result << "PC: " << std::setw(4) << std::setfill('0') << static_cast<int>(read("PC")) << '\n';

		result << std::string(15, '-') << '\n';
		result << "Z=" << read_flag("Z") << " N=" << read_flag("N") << " H=" << read_flag("H")
		       << " C=" << read_flag("C") << '\n';

		os << result.str();
	}

	void print() const
	{
		print(std::cout);
	}

	[[nodiscard]] auto dump() const
	{
		return register_array_;
	}

	void set_IME(const bool value)
	{
		ime_flag_ = value;
	}

	[[nodiscard]] auto read_IME() const
	{
		return ime_flag_;
	}
	//
	// TODO: Make it standalone function
	static auto register_index(const std::string_view& reg_name) -> int
	{
		if (reg_name == "AF") { return 0; }
		if (reg_name == "F") { return 0; }
		if (reg_name == "A") { return 1; }

		if (reg_name == "BC") { return 2; }
		if (reg_name == "C") { return 2; }
		if (reg_name == "B") { return 3; }

		if (reg_name == "DE") { return 4; }
		if (reg_name == "E") { return 4; }
		if (reg_name == "D") { return 5; }

		if (reg_name == "HL") { return 6; }
		if (reg_name == "L") { return 6; }
		if (reg_name == "H") { return 7; }

		if (reg_name == "PC") { return 8; }
		if (reg_name == "SP") { return 10; }

		assert(false && "Used register doesn't exist.");
	}

	auto operator==(const Registers& other) const
	{
		return register_array_ == other.register_array_ && ime_flag_ == other.ime_flag_;
	}

private:
	std::array<uint8_t, 12> register_array_ = {};

	bool ime_flag_ = false;
};


// TODO: Actually do snapshots, do not save whole state
class RegistersSnaphost {
public:
	RegistersSnaphost(const Registers& regs) : states_{regs} {}

	void add(const Registers& regs)
	{
		states_.push_back(regs);
	}

	auto get(const uint64_t steps)
	{
		assert(steps <= states_.size() - 1);
		return *(crbegin(states_) + steps);
	}

private:
	std::vector<Registers> states_;
};

inline auto operator<<(std::ostream& os, const Registers& registers) -> std::ostream&
{
	registers.print(os);
	return os;
}

struct MakeRegisters {
	std::optional<uint16_t> AF = {};
	std::optional<uint8_t> A = {};
	std::optional<uint8_t> F = {};
	std::optional<uint16_t> BC = {};
	std::optional<uint8_t> B = {};
	std::optional<uint8_t> C = {};
	std::optional<uint16_t> DE = {};
	std::optional<uint8_t> D = {};
	std::optional<uint8_t> E = {};
	std::optional<uint16_t> HL = {};
	std::optional<uint8_t> H = {};
	std::optional<uint8_t> L = {};
	std::optional<uint16_t> PC = {};
	std::optional<uint16_t> SP = {};

	std::optional<bool> IME = {};

	[[nodiscard]] auto get() const
	{
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

		const auto IME_val = IME.value_or(false);

		const auto registers_array = std::array<uint8_t, 12>{F_val, A_val, C_val, B_val, E_val, D_val, L_val, H_val};
		auto registers = Registers{registers_array};
		registers.write("PC", PC_val);
		registers.write("SP", SP_val);
		registers.set_IME(IME_val);
		return registers;
	}

	void check_consistency() const
	{
		assert(
		  !(AF.has_value() && (A.has_value() || F.has_value())) && "You can't set AF and A (or F) at the same time");
		assert(
		  !(BC.has_value() && (B.has_value() || C.has_value())) && "You can't set BC and B (or C) at the same time");
		assert(
		  !(DE.has_value() && (D.has_value() || E.has_value())) && "You can't set DE and D (or E) at the same time");
		assert(
		  !(HL.has_value() && (H.has_value() || L.has_value())) && "You can't set HL and H (or L) at the same time");
	}
};

// TODO: Merge from MakeRegisters or inherit?
struct RegistersChanger {
	std::optional<uint16_t> AF = {};
	std::optional<uint8_t> A = {};
	std::optional<uint8_t> F = {};
	std::optional<uint16_t> BC = {};
	std::optional<uint8_t> B = {};
	std::optional<uint8_t> C = {};
	std::optional<uint16_t> DE = {};
	std::optional<uint8_t> D = {};
	std::optional<uint8_t> E = {};
	std::optional<uint16_t> HL = {};
	std::optional<uint8_t> H = {};
	std::optional<uint8_t> L = {};
	std::optional<uint16_t> PC = {};
	std::optional<uint16_t> SP = {};

	std::optional<bool> IME = {};

	[[nodiscard]] auto get(const Registers& registers) const
	{
		check_consistency();
		auto changed_regs = registers;
		if (AF.has_value()) { changed_regs.write("AF", AF.value()); };
		if (A.has_value()) { changed_regs.write("A", A.value()); }
		if (F.has_value()) { changed_regs.write("F", F.value()); }
		if (BC.has_value()) { changed_regs.write("BC", BC.value()); }
		if (B.has_value()) { changed_regs.write("B", B.value()); }
		if (C.has_value()) { changed_regs.write("C", C.value()); }
		if (DE.has_value()) { changed_regs.write("DE", DE.value()); }
		if (D.has_value()) { changed_regs.write("D", D.value()); }
		if (E.has_value()) { changed_regs.write("E", E.value()); }
		if (HL.has_value()) { changed_regs.write("HL", HL.value()); }
		if (H.has_value()) { changed_regs.write("H", H.value()); }
		if (L.has_value()) { changed_regs.write("L", L.value()); }
		if (PC.has_value()) { changed_regs.write("PC", PC.value()); }
		if (SP.has_value()) { changed_regs.write("SP", SP.value()); }
		if (IME.has_value()) { changed_regs.set_IME(IME.value()); }
		return changed_regs;
	}

	void check_consistency() const
	{
		assert(
		  !(AF.has_value() && (A.has_value() || F.has_value())) && "You can't change AF and A (or F) at the same time");
		assert(
		  !(BC.has_value() && (B.has_value() || C.has_value())) && "You can't change BC and B (or C) at the same time");
		assert(
		  !(DE.has_value() && (D.has_value() || E.has_value())) && "You can't change DE and D (or E) at the same time");
		assert(
		  !(HL.has_value() && (H.has_value() || L.has_value())) && "You can't change HL and H (or L) at the same time");
	}
};

struct MakeFlags {
	const std::optional<bool> Z = {};
	const std::optional<bool> N = {};
	const std::optional<bool> H = {};
	const std::optional<bool> C = {};
	const std::optional<uint8_t> unused = {};

	[[nodiscard]] auto get() const
	{
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
	const std::optional<bool> Z = {};
	const std::optional<bool> N = {};
	const std::optional<bool> H = {};
	const std::optional<bool> C = {};
	const std::optional<uint8_t> unused = {};

	[[nodiscard]] auto get(const uint8_t orig_flags) const
	{
		const auto Z_val = Z.value_or(static_cast<bool>(orig_flags & (1 << 7)));
		const auto N_val = N.value_or(static_cast<bool>(orig_flags & (1 << 6)));
		const auto H_val = H.value_or(static_cast<bool>(orig_flags & (1 << 5)));
		const auto C_val = C.value_or(static_cast<bool>(orig_flags & (1 << 4)));
		const auto unused_val = unused.value_or(static_cast<uint8_t>(orig_flags & 0x0F));

		return MakeFlags{.Z = Z_val, .N = N_val, .H = H_val, .C = C_val, .unused = unused_val}.get();
	}
};
