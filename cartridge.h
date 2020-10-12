#pragma once

#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cstddef>
#include <iomanip>

// TODO: Move into utils file
template<typename TInput, typename TOutput>
auto convert(const TInput& source, TOutput& destination) {
	destination.clear();
	destination.resize(source.size());

	std::transform(
		begin(source),
		end(source),
		std::begin(destination),
		[](const auto& el) { return static_cast<typename TOutput::value_type>(el); }
	);
}

class Cartridge {
public:
	Cartridge(const std::string& filename) {
		auto file = std::ifstream(filename, std::ios::binary);
		auto buffer = std::vector<uint8_t>(std::istreambuf_iterator<char>(file), {});

		convert(buffer, buffer_);
	}

	// TODO: Move these print* methods elsewhere?
	void print(const uint16_t start, const uint16_t end) {
		for (auto i = start; i < end; ++i) {
			std::cout << std::to_integer<char>(buffer_[i]);
		}
		std::cout << '\n';
	}

	void print_as_hex(const uint16_t start, const uint16_t end) {
		std::cout << std::hex;
		for (auto i = start; i < end; ++i) {
			std::cout << std::to_integer<int>(buffer_[i]);
		}
		std::cout << '\n';
		std::cout << std::dec;
	}

	void print_as_int(const uint16_t start, const uint16_t end) {
		for (auto i = start; i < end; ++i) {
			std::cout << std::to_integer<int>(buffer_[i]);
		}
		std::cout << '\n';
	}


	auto get_as_string(const uint16_t start, const uint16_t end) {
		auto bytes = std::vector(begin(buffer_) + start, begin(buffer_) + end);
		auto bytes_as_string = std::string{};
		convert(bytes, bytes_as_string);
		return bytes_as_string;
	}

	auto get_title() {
		return get_as_string(0x0134, 0x0134 + 11);
	}

	auto get_manufacturer_code() {
		return get_as_string(0x13F, 0x13F + 4);
	}

	void print_hex_logo() {
		const auto start = 0x104;
		const auto end = 0x134;

		std::cout << std::hex;
		auto line_counter = 1;
		for(auto i = start; i < end; ++i, line_counter++) {
			std::cout << std::setw(2) << std::to_integer<int>(buffer_[i]) << ' ';
			if (line_counter % 16 == 0) std::cout << '\n';
		}
		std::cout << '\n';
		std::cout << std::dec;
	}

private:
	std::vector<std::byte> buffer_;
};
