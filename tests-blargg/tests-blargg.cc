#include "emulator.h"

#include <iostream>

auto main(int argc, char *argv[]) -> int
{
	if (argc != 4) {
		std::cout << "Usage " << argv[0] << " test_rom instructions_count expected_output\n";
		return 1;
	}

	const auto test_rom = std::string(argv[1]);
	const auto instructions_count = static_cast<uint64_t>(std::stoi(argv[2]));
	const auto * const expected_output = argv[3];

	const auto test_name = test_rom.substr(test_rom.rfind('/') + 1);

	std::cout << "Test " << test_name << ' ';
	std::cout << std::setw(30) << std::setfill('.') << " ";

	auto emu = Emulator<true>{test_rom};
	emu.execute_instructions(instructions_count);
	const auto serial_link_output = emu.get_serial_link();

	if (serial_link_output == expected_output) {
		std::cout << "Passed\n";
		return 0;
	}
	else {
		std::cout << "Failed\n";
		std::cout << "output:\n";
		std::cout << serial_link_output;
		std::cout << "\nshould be:\n";
		std::cout << expected_output;
		return 1;
	}
}
