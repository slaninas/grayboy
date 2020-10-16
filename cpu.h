#pragma once

#include <vector>
#include <iostream>

// TODO: Zero registers or do not initialize at all or like original ROM?
// TODO: Add unit tests for registers
struct Registers{
	std::array<uint8_t, 12> registers = {};
private:
	// TODO: Better way of doing this?
	uint16_t* AF_ptr = reinterpret_cast<uint16_t*>(registers.data() + 0);
	uint8_t* F_ptr= registers.data() + 0;
	uint8_t* A_ptr= registers.data() + 1;

	uint16_t* BC_ptr = reinterpret_cast<uint16_t*>(registers.data() + 2);
	uint8_t* C_ptr = registers.data() + 2;
	uint8_t* B_ptr = registers.data() + 3;

	uint16_t* DE_ptr = reinterpret_cast<uint16_t*>(registers.data() + 4);
	uint8_t* E_ptr = registers.data() + 4;
	uint8_t* D_ptr = registers.data() + 5;

	uint16_t* HL_ptr = reinterpret_cast<uint16_t*>(registers.data() + 6);
	uint8_t* L_ptr = registers.data() + 6;
	uint8_t* H_ptr = registers.data() + 7;

	uint16_t* PC_ptr = reinterpret_cast<uint16_t*>(registers.data()  + 8);
	uint16_t* SP_ptr = reinterpret_cast<uint16_t*>(registers.data()  + 10);

public:
	Registers()  {}
	Registers(const std::array<uint8_t, 12>& regs) : registers{regs} {}

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

	void print_regs() {
		auto print_pair = [](const auto& name, const auto& both, const auto& hi, const auto& lo) {
			std::cout << std::hex;
			std::cout << name << ": " << static_cast<int>(both);
			std::cout << " [" << static_cast<int>(hi) << ' ' << static_cast<int>(lo) << ']';
			std::cout << '\n';
			std::cout << std::dec;
		};

		std::cout << "Registers: \n" << std::string(20, '-') << '\n';
		print_pair("AF", regs_.AF, regs_.A, regs_.F);
		print_pair("BC", regs_.BC, regs_.B, regs_.C);
		print_pair("DE", regs_.DE, regs_.D, regs_.E);
		print_pair("HL", regs_.HL, regs_.H, regs_.L);

		std::cout << "PC: " << static_cast<int>(regs_.PC) << '\n';
		std::cout << "SP: " << static_cast<int>(regs_.SP) << '\n';

	}


	auto execute_next() {
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
