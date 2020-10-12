#include <iostream>
#include <vector>

#include "cartridge.h"

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
	std::cout << cart.get_title() << '\n';
}
