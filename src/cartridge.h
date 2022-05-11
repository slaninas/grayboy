#pragma once

#include <fstream>
#include <iomanip>
#include <iostream>
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

enum class MemoryBanking {
	NO_BANKING,
	MBC1,
	MBC2,
};

class Cartridge {
public:
	Cartridge() = default;
	Cartridge(const std::string& filename)
	{
		auto file = std::ifstream(filename, std::ios::binary);
		if (file.fail()) { throw std::invalid_argument(std::string("Can't open file >") + filename + "<"); }
		buffer_ = std::vector<uint8_t>(std::istreambuf_iterator<char>(file), {});

		const auto mbc = read(0x147);
		switch (mbc) {
			case 0:
				memory_banking_type_ = MemoryBanking::NO_BANKING;
				std::cout << "INFO: MemoryBanking::NO_BANKING;\n";
				break;
			case 1:
			case 2:
			case 3:
				memory_banking_type_ = MemoryBanking::MBC1;
				std::cout << "INFO: MemoryBanking::MBC1;\n";
				break;
			case 5:
			case 6:
				memory_banking_type_ = MemoryBanking::MBC2;
				std::cout << "INFO: MemoryBanking::MBC2;\n";
				throw std::exception{};
				break;
		}

		total_ram_banks_ = read(0x147);
		std::cout << "INFO: total_ram_banks_ " << (int)total_ram_banks_ << '\n';
		ram_banks_.resize(total_ram_banks_);

		for (auto& ram_bank : ram_banks_) { std::fill(begin(ram_bank), end(ram_bank), 0x00); }
	}

	// Banking: http://www.codeslinger.co.uk/pages/projects/gameboy/banking.html
	//          https://gbdev.io/pandocs/MBC1.html
	auto write(const uint16_t& address, const uint8_t& val) -> void
	{
		// Enable ram banking
		if (address <= 0x1fff) {
			if (memory_banking_type_ == MemoryBanking::NO_BANKING) { return; }

			if (memory_banking_type_ == MemoryBanking::MBC2) {
				if ((address & (1 << 4)) >> 4 == 1) return;
			}

			if ((val & 0xf) == 0xA) { ram_banking_enabled_ = true; }
			else if ((val & 0xf) == 0x0) {
				ram_banking_enabled_ = false;
			}
		}
		else if (address >= 0x2000 && address <= 0x3fff) {
			if (memory_banking_type_ == MemoryBanking::MBC1 || memory_banking_type_ == MemoryBanking::MBC2) {
				// Change ROM bank
				if (memory_banking_type_ == MemoryBanking::MBC2) {
					current_rom_bank_ = val & 0xf;
					if (current_rom_bank_ == 0) { current_rom_bank_ = 1; }
					return;
				}

				const auto lower5 = val & 31;
				current_rom_bank_ &= 224;
				current_rom_bank_ = lower5;
				if (current_rom_bank_ == 0) { current_rom_bank_ = 1; }
			}
		}
		else if (address >= 0x4000 && address <= 0x5fff) {
			if (memory_banking_type_ == MemoryBanking::MBC1) {
				if (rom_banking_) {
					// change hi ROM bank
					current_rom_bank_ &= 31;
					const auto val_crop = val & 224;
					current_rom_bank_ |= val_crop;

					if (current_rom_bank_ == 0) { current_rom_bank_ = 1; }
				}
				else {
					current_ram_bank_ = val & 0x3;
				}
			}
		}
		else if (address >= 0x6000 && address <= 0x7fff) {
			if (memory_banking_type_ == MemoryBanking::MBC1) {
				// change ROM RAM mode
				const auto val_crop = val & 0x1;
				rom_banking_ = val_crop == 0;
				if (rom_banking_) { current_ram_bank_ = 0; }
			}
		}

		else if (address >= 0xa000 && address <= 0xbfff) {
			if (ram_banking_enabled_) { ram_banks_[current_ram_bank_][address - 0xa000] = val; }
		}
	}

	auto read(const uint16_t& address) const -> uint8_t
	{
		if (address <= 0x3fff) { return buffer_[address]; }
		else if (address <= 0x7fff) {
			return buffer_[address - 0x4000 + (current_rom_bank_ * 0x4000)];
		}
		else if (address >= 0xa000 && address <= 0xbfff) {
			return ram_banks_[current_ram_bank_][address - 0xa000];
		}
		throw std::exception{};
		return 0;
	}

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
		auto sum = uint8_t{0};
		for (auto i = 0x134; i < 0x14C + 1; ++i) { sum -= buffer_[i] + 1; }
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

private:
	void print_as_hex(const std::pair<uint16_t, uint16_t>& range)
	{
		const auto [start, end] = range;
		std::cout << std::hex;
		for (auto i = start; i < end; ++i) { std::cout << buffer_[i]; }
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
			std::cout << std::setw(2) << buffer_[i] << ' ';
			if (line_counter % 16 == 0) { std::cout << '\n'; }
		}
		std::cout << '\n';
		std::cout << std::dec;
	}

	std::vector<uint8_t> buffer_ = {};
	MemoryBanking memory_banking_type_ = {};

	bool rom_banking_ = true;
	uint8_t current_rom_bank_ = 1;

	bool ram_banking_enabled_ = {};
	uint8_t current_ram_bank_ = {};
	uint8_t total_ram_banks_ = {};

	std::vector<std::array<uint8_t, 0x2000>> ram_banks_ = {};
};
