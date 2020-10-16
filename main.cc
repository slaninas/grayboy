#include <iostream>
#include <vector>
#include <array>

#include "cartridge.h"
#include "cpu.h"

template<typename T>
void p(const std::vector<T>& vec, const size_t num_values) {
	for (size_t i = 0; i < num_values; ++i) {
		std::cout << (char)vec[i];
	}
	std::cout << '\n';
}

template<typename T>
void p(const std::vector<T>& vec) {
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
	cpu.get_registers_dump().print();
	cpu.execute_next();
	cpu.get_registers_dump().print();
}
