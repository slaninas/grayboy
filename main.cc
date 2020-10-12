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
	std::cout << "Title: " << cart.get_title() << '\n';
	std::cout << "Manufacturer Code: " << cart.get_manufacturer_code() << '\n';
	// cart.print_hex_logo();

	std::cout << "CGB Flag: ";
	cart.print_as_hex(0x143, 0x143 + 1);

	std::cout << "ROM Size Code: ";
	cart.print_as_hex(0x148, 0x148 + 1);

	std::cout << "RAM Size Code (in cartridge): ";
	cart.print_as_hex(0x149, 0x149 + 1);

	std::cout << "Destination Code: ";
	cart.print_as_hex(0x14A, 0x14A + 1);

	std::cout << "Header Checksum: ";
	cart.print_as_hex(0x14D, 0x14D + 1);

	std::cout << std::hex;
	std::cout << "Calculated Header Checksum: " << cart.get_header_checksum() << '\n';
	std::cout << std::dec;

}
