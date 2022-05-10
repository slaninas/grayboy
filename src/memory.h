#pragma once

#include <array>

#include "cartridge.h"

template<typename T>
auto raw_dump(const T& container, const std::string& filename) {
	auto file = std::ofstream(filename, std::ios::binary);
	file.write(reinterpret_cast<const char*>(container.data()), container.size() * sizeof(typename T::value_type));
}

class Memory {
public:
	static const size_t ArrayElements = 1 << 16;
	using ArrayType = std::array<uint8_t, ArrayElements>;
	using AddressType = uint16_t;

	Memory() = default;

	Memory(Cartridge&& cartridge) {
		cartridge_ = cartridge;
		std::fill(begin(array_) + 0xa000, begin(array_) + 0xe000, 0xff);

		// Timer setup
		array_[0xff04] = 0xac;
		array_[0xff07] = 0xf8;
		array_[0xff40] = 0x91;

		// LCD setup
		array_[0xff40] = 0x91;
		array_[0xff41] = 0x80;

	}

	void direct_write(const uint16_t address, const uint8_t value)
	{
		array_[address] = value;
	}

	[[nodiscard]] auto direct_read(const uint16_t address) const
	{
		return array_[address];
	}

	[[nodiscard]] auto read(const uint16_t address) const -> uint8_t
	{

		if (address <= 0x7fff) {
			return cartridge_.read(address);
		}

		if (address == 0xff00) {
			if (~direct_read(0xff00) & (1 << 4)) {
				return static_cast<uint8_t>(get_direction_keys());
			} else if (~direct_read(0xff00) & (1 << 5)) {
				return static_cast<uint8_t>(get_button_keys());
			}
		}

		// This part of memory is not usable and always returns 0xff
		if (address >= 0xfea0 && address <= 0xfeff) {
			return 0xff;
		}
		return array_[address];
	}


	void write(const uint16_t address, const uint8_t value)
	{
		// ROM
		if (address <= 0x7fff || (address >= 0xa000 && address <= 0xbfff)) {
		// std::cout << "INFO: attempt to write to 0x" << std::hex << (int)address << " value 0x" << (int)value << std::dec << '\n';
			cartridge_.write(address, value);
		}
		// Scanline reset
		// if (address == 0xff44) {
			// array_[address] = 0;
		// }

		// DMA
		if (address == 0xff46) {
			const auto source = value << 8;
			for (auto i = 0; i < 0xa0; ++i) {
				array_[0xfe00 + i] = array_[source + i];
			}
		}
		// Write to DIV resets it
		else if (address == 0xff04) {
			array_[address] = 0x00;
		}

		// This part of memory is not usable
		else if (address >= 0xfea0 && address <= 0xfeff) {
			return;
		}
		else {
			array_[address] = value;
		}
	}

	[[nodiscard]] auto dump() const
	{
		return array_;
	}

	auto update_joypad(const uint8_t& new_state) -> void {
		joypad_state_ = new_state;
	}

private:

	auto get_direction_keys() const -> uint8_t {
		return ~(joypad_state_ & 0x0f);
	}
	auto get_button_keys() const -> uint8_t {
		return ~((joypad_state_ >> 4) & 0x0f);
	}

	ArrayType array_ = {};
	Cartridge cartridge_ = {};
	uint8_t joypad_state_ = {};
};

