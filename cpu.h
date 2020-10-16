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
		std::cout << std::dec;
	}



private:
	Registers regs_;
	uint8_t flags_;

	std::vector<Instruction> instructions_ = {
		{"LD BC, d16", 0x01, 3, 3}
	};
};
