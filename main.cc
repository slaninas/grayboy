// For debug
#include <iostream>
#include <vector>
#include <array>

#include "cartridge.h"
#include "cpu.h"
#include "registers.h"
#include "registers.h"
#include "curse.h"

void dprint(const DissasemblyInfo& info, std::ostream& os) {
	os << std::hex;
	os << "0x" << info.address << ": " << info.instruction.mnemonic << " | ";
	for (const auto& val : info.memory_representation) {
		os << (int)val << ' ';
	}
	os << '\n';
	os << std::dec;
}

void dprint(const DissasemblyInfo& info) {
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

auto disasseble(Cpu& cpu) {
	auto addr = static_cast<uint16_t>(0x100);

	auto dissasembled = std::vector<DissasemblyInfo>{};

	for (int i = 0; i < 1000; ++i) {
		const auto info =  cpu.disassemble_next(addr);
		addr = info.next_address;
		dissasembled.push_back(info);
	}

	std::sort(
		begin(dissasembled),
		end(dissasembled),
		[](const auto& a, const auto& b) {
			return a.address < b.address;
		}
	);
	auto new_end = std::unique(begin(dissasembled), end(dissasembled), [] (const auto& a, const auto&b) { return a.address == b.address; } );

	dissasembled.erase(new_end, end(dissasembled));
	p(dissasembled);
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
		const auto dissasembled = cpu.disassemble_next(next_addr);
		cpu.registers().print();
		std::cout << "Next: "; dprint(dissasembled);
		next_addr = dissasembled.next_address;
		cpu.execute_next();
		std::cin >> c;
		while (c == 'm') {
			print_memory(cpu.memory_dump());
			std::cin >> c;
		}
	}

	auto cursed = MainCurse{};
	auto win = CursedWindow{{40, 10}, {30, 30}};
	// cursed.add_window(std::move(win));

	while (1) {
		auto registers_ss = std::ostringstream{};
		auto ss = std::ostringstream{};

		const auto dissasembled = cpu.disassemble_next(next_addr);
		next_addr = dissasembled.next_address;
		ss << "Next: ";
		dprint(dissasembled, ss);
		cursed.add(ss.str());

		cpu.registers().print(registers_ss);

		win.update(registers_ss.str());
		cursed.wait_for_any();

		cpu.execute_next();
	}

}
