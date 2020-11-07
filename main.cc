// For debug
#include <iostream>
#include <vector>
#include <array>

#include "cartridge.h"
#include "cpu.h"
#include "registers.h"
#include "registers.h"
#include "curse.h"

void dprint(const DisassemblyInfo& info, std::ostream& os) {
	os << std::hex;
	os << "0x" << info.address << ": " << info.instruction.mnemonic << " | ";
	for (const auto& val : info.memory_representation) {
		os << (int)val << ' ';
	}
	os << '\n';
	os << std::dec;
}

void dprint(const DisassemblyInfo& info) {
	dprint(info, std::cout);
}


template<typename T>
void p(const T& cont) {
	for (const auto el : cont) {
		dprint(el);
	}
}

template<typename T>
void print_memory(const T& mem) {

	for (int i = 0; i < mem.size(); ++i) {
		const auto& value = mem[i];
		if (value != 0) {
			std::cout << "Memory 0x" << i << "=" << (int)value <<'\n';
		}
	}
}

auto disassemble(Cpu& cpu) {
	auto addr = static_cast<uint16_t>(0x100);

	auto disassembled = std::vector<DisassemblyInfo>{};

	for (int i = 0; i < 1000; ++i) {
		const auto info =  cpu.disassemble_next(addr);
		addr = info.next_address;
		disassembled.push_back(info);
	}

	std::sort(
		begin(disassembled),
		end(disassembled),
		[](const auto& a, const auto& b) {
			return a.address < b.address;
		}
	);
	auto new_end = std::unique(begin(disassembled), end(disassembled), [] (const auto& a, const auto&b) { return a.address == b.address; } );

	disassembled.erase(new_end, end(disassembled));
	return disassembled;
}

auto get_from_to(const std::vector<DisassemblyInfo>& infos, const uint16_t& from, const uint16_t& to, const uint16_t& next_addr, std::ostream& os) {
	for (const auto info : infos) {
		auto character = info.address == next_addr ? '>' : ' ';
		os << character;
		dprint(info, os);
	}
}

int main(int argc, const char** argv) {
	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " cartridge_filename\n";
		return 1;
	}

	auto filename = std::string(argv[1]);

	auto cart = Cartridge{filename};
	// cart.print_info();

	const auto rom = cart.dump();
	auto array = Memory::ArrayType{};

	std::transform(
		begin(rom),
		end(rom),
		begin(array),
		[](const auto& el) { return static_cast<uint8_t>(el); }
	);

	const auto regs = RegistersChanger{.PC=0x0100}.get(Registers{});

	auto cpu = Cpu{array, regs};
	cpu.registers().print();

	// disasseble(cpu);
	char c;
	auto next_addr = static_cast<uint16_t>(0x0100);
	uint16_t num;

	while (0) {
		const auto disassembled = cpu.disassemble_next(next_addr);
		cpu.registers().print();
		std::cout << "Next: "; dprint(disassembled);
		next_addr = disassembled.next_address;
		cpu.execute_next();
		std::cin >> c;
		while (c == 'm') {
			print_memory(cpu.memory_dump());
			std::cin >> c;
		}
	}

	auto cursed = MainCurse{};
	auto reg_window = CursedWindow{{40, 10}, {18, 12}};
	auto instruction_window = CursedWindow{{0, 0}, {40, 50}};
	auto instruction_window2 = CursedWindow{{60, 0}, {40, 50}};

	auto cpu_copy = cpu;
	auto disassembled_more = disassemble(cpu_copy);


	while (1) {
		auto registers_ss = std::ostringstream{};
		auto ss = std::ostringstream{};
		auto ss2 = std::ostringstream{};

		const auto disassembled = cpu.disassemble_next(next_addr);
		next_addr = disassembled.next_address;
		ss << "Next: ";
		dprint(disassembled, ss);
		// cursed.add(ss.str());

		cpu.registers().print(registers_ss);

		reg_window.update(registers_ss.str());
		instruction_window.add(ss.str());

		get_from_to(disassembled_more, 0, 0, cpu.registers().read("PC"), ss2);

		instruction_window2.update(ss2.str());
		cursed.wait_for_any();

		cpu.execute_next();
	}

}
