#pragma once

#include <iostream>
#include <stdexcept>
#include <cassert>

// TODO: Zero registers or do not initialize at all or like original ROM?
// TODO: Add unit tests for registers
// TODO: Use assert and run in debug instead of throwing when it makes sense
class Registers{
public:
	using ArrayType = std::array<uint8_t, 12>;

	Registers() = default;
	Registers(const ArrayType& regs_array) : register_array_{regs_array} {}

	void clear() {
		std::fill(begin(register_array_), end(register_array_), 0x0);
	}

	// TODO: Maybe use macro? Rather do it differently
	[[nodiscard]] auto read_AF() const { return *reinterpret_cast<const uint16_t*>(register_array_.data() + 0);}
	[[nodiscard]] auto read_F() const {	return register_array_[0]; }
	[[nodiscard]] auto read_A() const {	return register_array_[1]; }

	[[nodiscard]] auto read_BC() const { return *reinterpret_cast<const uint16_t*>(register_array_.data() + 2);}
	[[nodiscard]] auto read_C() const {	return register_array_[2]; }
	[[nodiscard]] auto read_B() const {	return register_array_[3]; }

	[[nodiscard]] auto read_DE() const { return *reinterpret_cast<const uint16_t*>(register_array_.data() + 4);}
	[[nodiscard]] auto read_E() const {	return register_array_[4]; }
	[[nodiscard]] auto read_D() const {	return register_array_[5]; }

	[[nodiscard]] auto read_HL() const { return *reinterpret_cast<const uint16_t*>(register_array_.data() + 6);}
	[[nodiscard]] auto read_L() const {	return register_array_[6]; }
	[[nodiscard]] auto read_H() const {	return register_array_[7]; }

	[[nodiscard]] auto read_PC() const { return *reinterpret_cast<const uint16_t*>(register_array_.data() + 8);}
	[[nodiscard]] auto read_SP() const { return *reinterpret_cast<const uint16_t*>(register_array_.data() + 10);}


	void write_AF(uint16_t value) { *reinterpret_cast<uint16_t*>(register_array_.data() + 0) = value; }
	void write_F(uint8_t value) {	register_array_[0] = value; }
	void write_A(uint8_t value) {	register_array_[1] = value; }

	void write_BC(uint16_t value) { *reinterpret_cast<uint16_t*>(register_array_.data() + 2) = value; }
	void write_C(uint8_t value) {	register_array_[2] = value; }
	void write_B(uint8_t value) {	register_array_[3] = value; }

	void write_DE(uint16_t value) { *reinterpret_cast<uint16_t*>(register_array_.data() + 4) = value; }
	void write_E(uint8_t value) {	register_array_[4] = value; }
	void write_D(uint8_t value) {	register_array_[5] = value; }

	void write_HL(uint16_t value) { *reinterpret_cast<uint16_t*>(register_array_.data() + 6) = value; }
	void write_L(uint8_t value) {	register_array_[6] = value; }
	void write_H(uint8_t value) {	register_array_[7] = value; }

	void write_PC(uint16_t value) { *reinterpret_cast<uint16_t*>(register_array_.data() + 8) = value; }
	void write_SP(uint16_t value) { *reinterpret_cast<uint16_t*>(register_array_.data() + 10) = value; }

	template<size_t kSize>
	[[nodiscard]] auto read(const char(&reg_name)[kSize]) {
		constexpr auto reg_name_size = kSize - 1; // Subtract \0 at the end
		const auto reg_index = register_index(reg_name);

		// 8bit registers
		if constexpr(reg_name_size == 1) {
			return register_array_[reg_index];;
		}
		// Combined 16bit registers
		else {
			return *reinterpret_cast<uint16_t*>(register_array_.data() + reg_index);
		}
	}

	// TODO: check if I'm not trying to write 16bit value into 8bit
	template<size_t kSize>
	void write(const char(&reg_name)[kSize], uint16_t value) {
		constexpr auto reg_name_size = kSize - 1; // Subtract \0 at the end
		const auto reg_index = register_index(reg_name);

		// 8bit registers
		if constexpr(reg_name_size == 1) {
			assert(((value & static_cast<uint16_t>(0xff00)) == 0) && "Don't write 16bit value into 8bit register.");
			register_array_[reg_index] = value;
		}
		// Combined 16bit registers
		else {
			*reinterpret_cast<uint16_t*>(register_array_.data() + reg_index) = value;
		}
	}


	auto& print(std::ostream& os) const {
		os << std::hex;
		auto print_pair = [&](const auto& name, const auto& both, const auto& hi, const auto& lo) {
			os << name << ": " << static_cast<int>(both);
			os << " [" << static_cast<int>(hi) << ' ' << static_cast<int>(lo) << ']';
			os << '\n';
		};

		os << "Registers: \n" << std::string(20, '-') << '\n';
		print_pair("AF", read_AF(), read_A(), read_F());
		print_pair("BC", read_BC(), read_B(), read_C());
		print_pair("DE", read_DE(), read_D(), read_E());
		print_pair("HL", read_HL(), read_H(), read_L());

		os << "PC: " << static_cast<int>(read_PC()) << '\n';
		os << "SP: " << static_cast<int>(read_SP()) << '\n';

		os << std::dec;
		return os;
	}

	void print() const {
		print(std::cout);
	}

	// TODO: remove
	auto array_copy() const {
		return register_array_;
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

// TODO: When AF, A and B should not be set etc., add unit test for that
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
		registers.write_PC(PC_val);
		registers.write_SP(SP_val);
		return registers;
	}

	void check_consistency() {
		if (AF.has_value()) {
			if (A.has_value() && (AF.value() & 0xFF00) >> 8 != A.value()) {
				throw std::logic_error("Value in AF register doesn't correspond to the value in A");
			}
			if (F.has_value() && (AF.value() & 0x00FF) != B.value()) {
				throw std::logic_error("Value in AF register doesn't correspond to the value in F");
			}
		}

		if (BC.has_value()) {
			if (B.has_value() && (BC.value() & 0xFF00) >> 8 != B.value()) {
				throw std::logic_error("Value in BC register doesn't correspond to the value in B");
			}
			if (C.has_value() && (BC.value() & 0x00FF) != C.value()) {
				throw std::logic_error("Value in BC register doesn't correspond to the value in C");
			}
		}

		if (DE.has_value()) {
			if (D.has_value() && (DE.value() & 0xFF00) >> 8 != D.value()) {
				throw std::logic_error("Value in BC register doesn't correspond to the value in D");
			}
			if (D.has_value() && (DE.value() & 0x00FF) != E.value()) {
				throw std::logic_error("Value in BC register doesn't correspond to the value in E");
			}
		}

		if (HL.has_value()) {
			if (H.has_value() && (HL.value() & 0xFF00) >> 8 != H.value()) {
				throw std::logic_error("Value in BC register doesn't correspond to the value in H");
			}
			if (L.has_value() && (HL.value() & 0x00FF) != L.value()) {
				throw std::logic_error("Value in BC register doesn't correspond to the value in L");
			}
		}

	}

};
