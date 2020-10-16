#pragma once

#include <vector>

union RegisterPair {
	uint16_t all;
	struct {
		uint8_t lo;
		uint8_t hi;
	};
};

// TODO: Zero registers or do not initialize at all or like original ROM?
struct Registers {
	RegisterPair AF = {0xabcd};
	RegisterPair BC = {10};
	RegisterPair DE = {100};
	RegisterPair HL = {1000};
	uint16_t SP = 1234;
	uint16_t PC = 0;
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

	void print_regs() {
		auto print_pair = [](const auto& reg, const auto& name) {
			std::cout << name << ": ";
			std::cout << static_cast<int>(reg.all);
			std::cout  << " [" << static_cast<int>(reg.hi) << ", " <<  static_cast<int>(reg.lo) << "]\n";
		};

		std::cout << "Registers: \n" << std::string(20, '-') << '\n';
		std::cout << std::hex;
		print_pair(regs_.AF, "AF");
		print_pair(regs_.BC, "BC");
		print_pair(regs_.DE, "DE");
		print_pair(regs_.HL, "HL");
		std::cout << "SP: " << static_cast<int>(regs_.SP) << '\n';
		std::cout << "PC: " << static_cast<int>(regs_.PC) << '\n';
		std::cout << std::dec;
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
				// regs_.BC.hi = memory_[instruction_start + 1];
				// regs_.BC.lo = memory_[instruction_start + 2];
				regs_.BC.hi = 0xff;
				regs_.BC.lo = 0xee;
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

	// TODO: Implement memory
	std::array<uint8_t, 1 << 16> memory_ = {0x01, 0xab, 0xcd};

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
