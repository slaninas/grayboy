// For debug
#include <iostream>
#include <vector>
#include <array>

#include "cartridge.h"
#include "cpu.h"
#include "registers.h"
#include "registers.h"


template<typename T>
void p(const T& cont) {
	std::cout << std::hex;
	for (const auto el : cont) {
		std::cout << "0x" << el.address << ": " << el.instruction.mnemonic << " | ";
		for (const auto& val : el.memory_representation) {
			std::cout << (int)val << ' ';
		}
		std::cout << '\n';
	}
	std::cout << std::dec;
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
	// auto next = static_cast<uint16_t>(0x0100);
	// for (auto i = 0; i < 10; ++i) {
		// cpu.execute_next();
		// cpu.registers().print();
		// std::cout << "---------------------------\n\n";
	// }

	disasseble(cpu);
	cpu.registers().print();

}
