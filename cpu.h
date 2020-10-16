#pragma once

#include <vector>
#include <iostream>

// TODO: Zero registers or do not initialize at all or like original ROM?
// TODO: Add unit tests for registers
struct Registers{
	std::array<uint8_t, 12> register_array = {};
private:
	// TODO: Better way of doing this?
	uint16_t* AF_ptr = reinterpret_cast<uint16_t*>(register_array.data() + 0);
	uint8_t* F_ptr= register_array.data() + 0;
	uint8_t* A_ptr= register_array.data() + 1;

	uint16_t* BC_ptr = reinterpret_cast<uint16_t*>(register_array.data() + 2);
	uint8_t* C_ptr = register_array.data() + 2;
	uint8_t* B_ptr = register_array.data() + 3;

	uint16_t* DE_ptr = reinterpret_cast<uint16_t*>(register_array.data() + 4);
	uint8_t* E_ptr = register_array.data() + 4;
	uint8_t* D_ptr = register_array.data() + 5;

	uint16_t* HL_ptr = reinterpret_cast<uint16_t*>(register_array.data() + 6);
	uint8_t* L_ptr = register_array.data() + 6;
	uint8_t* H_ptr = register_array.data() + 7;

	uint16_t* PC_ptr = reinterpret_cast<uint16_t*>(register_array.data()  + 8);
	uint16_t* SP_ptr = reinterpret_cast<uint16_t*>(register_array.data()  + 10);

public:
	Registers() = default;
	Registers(const std::array<uint8_t, 12>& regs_array) : register_array{regs_array} {}

	void clear() {
		std::fill(begin(register_array), end(register_array), 0x0);
	}

	auto& print(std::ostream& os) const {
		auto print_pair = [&](const auto& name, const auto& both, const auto& hi, const auto& lo) {
			os << std::hex;
			os << name << ": " << static_cast<int>(both);
			os << " [" << static_cast<int>(hi) << ' ' << static_cast<int>(lo) << ']';
			os << '\n';
			os << std::dec;
		};

		os << "Registers: \n" << std::string(20, '-') << '\n';
		print_pair("AF", AF, A, F);
		print_pair("BC", BC, B, C);
		print_pair("DE", DE, D, E);
		print_pair("HL", HL, H, L);

		os << "PC: " << static_cast<int>(PC) << '\n';
		os << "SP: " << static_cast<int>(SP) << '\n';

		return os;
	}

	void print() const {
		print(std::cout);
	}

	uint16_t& AF = *AF_ptr;
	uint8_t& A = *A_ptr;
	uint8_t& F = *F_ptr;

	uint16_t& BC = *BC_ptr;
	uint8_t& B = *B_ptr;
	uint8_t& C = *C_ptr;

	uint16_t& DE = *DE_ptr;
	uint8_t& D = *D_ptr;
	uint8_t& E = *E_ptr;

	uint16_t& HL = *HL_ptr;
	uint8_t& H = *H_ptr;
	uint8_t& L = *L_ptr;

	uint16_t PC = *PC_ptr;
	uint16_t SP = *SP_ptr;
};

std::ostream& operator<<(std::ostream& os, const Registers& registers) {
	return registers.print(os);
}

struct Instruction {
	std::string mnemonic;
	uint16_t opcode;
	uint8_t size;
	uint8_t cycles;
};

// Make exceptions asserts and run in debug
// TODO: Add unit tests
class Cpu {
public:
	using MemoryType = std::array<uint8_t, 1 << 16>;

	Cpu(MemoryType&& memory) :
		memory_{memory}
	{}


	void clear_registers() {
		regs_.clear();
	}

	[[nodiscard]] auto execute_next() {
		std::cout << memory_.size() << '\n';

		const auto instruction_start = regs_.PC;
		const auto opcode = memory_[instruction_start];

		if (opcode == 0xBC) {
			throw std::runtime_error("16-bit opcodes not implemented yet.");
		}

		const auto instruction = find_by_opcode(opcode);

		switch(opcode) {
			case 0x01:
				regs_.B = memory_[instruction_start + 1];
				regs_.C = memory_[instruction_start + 2];
				break;
			default:
				throw std::runtime_error("Opcode not implemented yet.");
				break;
		}

		regs_.PC += instruction.size;
		return instruction.cycles;
	}

	[[nodiscard]] auto get_registers_dump() {
		return regs_;
	}


private:
	Registers regs_;
	uint8_t flags_;

	MemoryType memory_ = {};

	std::vector<Instruction> instructions_ = {
		{"LD BC, d16", 0x01, 3, 3},
		{"LD (BC), A", 0x02, 1, 2}
	};

	const Instruction& find_by_opcode(const uint16_t opcode) {
		auto res = std::find_if(begin(instructions_), end(instructions_), [opcode](const auto& instruction) { return instruction.opcode ==  opcode; });
		if (res == end(instructions_)) {
			throw std::runtime_error("Opcode not found");
		}
		return *res;
	}


};
