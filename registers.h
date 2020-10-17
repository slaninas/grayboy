#pragma once

// TODO: Zero registers or do not initialize at all or like original ROM?
// TODO: Add unit tests for registers
class Registers{
public:

	Registers() = default;
	Registers(const std::array<uint8_t, 12>& regs_array) : register_array_{regs_array} {}

	void clear() {
		std::fill(begin(register_array_), end(register_array_), 0x0);
	}

	// TODO: Maybe use macro?
	auto read_AF() const { return *reinterpret_cast<const uint16_t*>(register_array_.data() + 0);}
	auto read_F() const {	return register_array_[0]; }
	auto read_A() const {	return register_array_[1]; }

	auto read_BC() const { return *reinterpret_cast<const uint16_t*>(register_array_.data() + 2);}
	auto read_C() const {	return register_array_[2]; }
	auto read_B() const {	return register_array_[3]; }

	auto read_DE() const { return *reinterpret_cast<const uint16_t*>(register_array_.data() + 4);}
	auto read_E() const {	return register_array_[4]; }
	auto read_D() const {	return register_array_[5]; }

	auto read_HL() const { return *reinterpret_cast<const uint16_t*>(register_array_.data() + 6);}
	auto read_L() const {	return register_array_[6]; }
	auto read_H() const {	return register_array_[7]; }

	auto read_PC() const { return *reinterpret_cast<const uint16_t*>(register_array_.data() + 8);}
	auto read_SP() const { return *reinterpret_cast<const uint16_t*>(register_array_.data() + 10);}


	auto write_AF(uint16_t value) { *reinterpret_cast<uint16_t*>(register_array_.data() + 0) = value; }
	auto write_F(uint8_t value) {	register_array_[0] = value; }
	auto write_A(uint8_t value) {	register_array_[1] = value; }

	auto write_BC(uint16_t value) { *reinterpret_cast<uint16_t*>(register_array_.data() + 2) = value; }
	auto write_C(uint8_t value) {	register_array_[2] = value; }
	auto write_B(uint8_t value) {	register_array_[3] = value; }

	auto write_DE(uint16_t value) { *reinterpret_cast<uint16_t*>(register_array_.data() + 4) = value; }
	auto write_E(uint8_t value) {	register_array_[4] = value; }
	auto write_D(uint8_t value) {	register_array_[5] = value; }

	auto write_HL(uint16_t value) { *reinterpret_cast<uint16_t*>(register_array_.data() + 6) = value; }
	auto write_L(uint8_t value) {	register_array_[6] = value; }
	auto write_H(uint8_t value) {	register_array_[7] = value; }

	auto write_PC(uint16_t value) { *reinterpret_cast<uint16_t*>(register_array_.data() + 8) = value; }
	auto write_SP(uint16_t value) { *reinterpret_cast<uint16_t*>(register_array_.data() + 10) = value; }

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

	auto array_copy() const {
		return register_array_;
	}

private:
	std::array<uint8_t, 12> register_array_ = {};
};

std::ostream& operator<<(std::ostream& os, const Registers& registers) {
	return registers.print(os);
}

struct MakeRegisters{
	const uint8_t A;
	const uint8_t F;
	const uint8_t B;
	const uint8_t C;
	const uint8_t D;
	const uint8_t E;
	const uint8_t H;
	const uint8_t L;
	const uint16_t PC;
	const uint16_t SP;

	auto get() {
		auto array = std::array<uint8_t, 12>{F, A, C, B, E, D, L, H};
		auto registers = Registers{array};
		registers.write_PC(PC);
		registers.write_SP(SP);
		return registers;
	}

};
