#pragma once

#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <cstddef>

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

	void print(const uint16_t start, const uint16_t end) {
		for (auto i = start; i < end; ++i) {
			std::cout << std::to_integer<char>(buffer_[i]);
		}
		std::cout << '\n';
	}

	std::string get_title() {
		auto title_begin = begin(buffer_) + 0x0134;
		auto title_length = 15;
		auto title = std::vector(title_begin, title_begin + title_length);
		auto title_str = std::string{};

		convert(title, title_str);

		title_str.insert(11, "[");
		title_str.push_back(']');

		return title_str;
	}

private:
	std::vector<std::byte> buffer_;
};
