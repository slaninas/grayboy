#pragma once

#include <algorithm>
#include <cstddef>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <vector>

// TODO: Move into utils file
template<typename TInput, typename TOutput>
auto convert(const TInput& source, TOutput& destination)
{
	destination.clear();
	destination.resize(source.size());

	std::transform(begin(source), end(source), std::begin(destination), [](const auto& el) {
		return static_cast<typename TOutput::value_type>(el);
	});
}

// TODO: Add unit tests
// TODO: Add check for header checksum?
class Cartridge {
public:
	Cartridge(const std::string& filename)
	{
		auto file = std::ifstream(filename, std::ios::binary);
		if (file.fail()) {
			throw std::invalid_argument(std::string("Can't open file >") + filename + "<");
		}
		auto buffer = std::vector<uint8_t>(std::istreambuf_iterator<char>(file), {});

		convert(buffer, buffer_);
	}

	// TODO: Why can't it be const? At least make std::pair const?
	static inline std::map<const char*, std::pair<std::uint16_t, std::uint16_t>> addreses = {
	  {"nintendo_logo", {0x104, 0x134}},
	  {"title", {0x134, 0x13f}},
	  {"manufacturer_code", {0x13f, 0x143}},
	  {"cbg_flag", {0x143, 0x144}},
	  {"rom_size_code", {0x148, 0x149}},
	  {"ram_size_code", {0x149, 0x14a}},
	  {"destination_code", {0x14a, 0x14b}},
	  {"header_checksum", {0x14d, 0x14e}}};

	auto get_header_checksum()
	{
		unsigned char sum = 0;
		for (auto i = 0x134; i < 0x14C + 1; ++i) { sum -= std::to_integer<char>(buffer_[i]) + 1; }
		return static_cast<int>(sum);
	}

	void print_info()
	{
		std::cout << "Cartridge Info:\n";
		std::cout << std::string(20, '-') << '\n';

		std::cout << "Title: ";
		print_as_string(addreses["title"]);
		std::cout << '\n';
		std::cout << "Manufacturer Code: ";
		print_as_string(addreses["manufacturer_code"]);
		std::cout << '\n';
		std::cout << "CBG Flag: ";
		print_as_hex(addreses["cbg_flag"]);
		std::cout << '\n';
		std::cout << "ROM Size Code: ";
		print_as_hex(addreses["rom_size_code"]);
		std::cout << '\n';
		std::cout << "RAM Size Code (in cartridge): ";
		print_as_hex(addreses["ram_size_code"]);
		std::cout << '\n';
		std::cout << "Destination Code: ";
		print_as_hex(addreses["destination_code"]);
		std::cout << '\n';
		std::cout << "Header Checksum: ";
		print_as_hex(addreses["header_checksum"]);
		std::cout << '\n';

		std::cout << std::hex;
		std::cout << "Calculated Header Checksum: " << get_header_checksum() << '\n';
		std::cout << std::dec;
	}

	auto dump() const
	{
		return buffer_;
	}

private:
	void print_as_hex(const std::pair<uint16_t, uint16_t>& range)
	{
		const auto [start, end] = range;
		std::cout << std::hex;
		for (auto i = start; i < end; ++i) { std::cout << std::to_integer<int>(buffer_[i]); }
		std::cout << std::dec;
	}

	void print_as_string(const std::pair<uint16_t, uint16_t>& range)
	{
		const auto [start, end] = range;
		auto bytes = std::vector(begin(buffer_) + start, begin(buffer_) + end);
		auto bytes_as_string = std::string{};
		convert(bytes, bytes_as_string);
		std::cout << bytes_as_string;
	}

	void print_hex_logo()
	{
		const auto [start, end] = addreses["nintendo_logo"];

		std::cout << std::hex;
		auto line_counter = 1;
		for (auto i = start; i < end; ++i, line_counter++) {
			std::cout << std::setw(2) << std::to_integer<int>(buffer_[i]) << ' ';
			if (line_counter % 16 == 0) { std::cout << '\n'; }
		}
		std::cout << '\n';
		std::cout << std::dec;
	}

	// TODO: Use uint8_t
	std::vector<std::byte> buffer_;
};
