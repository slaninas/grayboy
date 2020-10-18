#include <iostream>
#include <vector>
#include <array>

#include "cartridge.h"
#include "cpu.h"

template<typename T>
void p(const T& vec, const size_t num_values) {
	std::cout << std::hex;
	for (size_t i = 0; i < num_values; ++i) {
		std::cout << (int)vec[i] << ' ';
	}
	std::cout << '\n';
	std::cout << std::dec;
}

template<typename T>
void p(const T& vec) {
	p(vec, vec.size());
}

int main(int argc, const char** argv) {
	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " cartridge_filename\n";
		return 1;
	}

	auto filename = std::string(argv[1]);

	auto cart = Cartridge{filename};
	// cart.print_info();

	auto memory = std::array<uint8_t, 1 << 16>{0x01, 0xAB, 0xCB};
	auto cpu = Cpu{std::move(memory)};

	auto regs = MakeRegisters{.F=0xff}.get();
	// auto regs = MakeRegisters{.F=0x00}.get();
	regs.print();
	// regs.set_flag("Z", 1);
	regs.set_flag("Z", 0);
	regs.print();

}
