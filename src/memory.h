#pragma once

#include <cstdint>
#include <array>
#include <cassert>
#include <vector>
#include <fstream>

#include "cartridge.h"

struct MemoryDiff {
	uint16_t address;
	uint8_t orig_value;
	uint8_t new_value;
};

inline bool operator==(const MemoryDiff& a, const MemoryDiff& b) {
	return a.address == b.address
		&& a.orig_value == b.orig_value
		&& a.new_value == b.new_value;
}

template<typename Memory>
auto memory_diff(const Memory& orig_memory, const Memory& new_memory)
{
	auto result = std::vector<MemoryDiff>{};

	for (auto address = static_cast<size_t>(0); address < Memory::ArrayElements; ++address) {
		const auto orig_value = orig_memory.read(address);
		const auto new_value = new_memory.read(address);
		if (orig_value != new_value) { result.push_back({static_cast<uint16_t>(address), orig_value, new_value}); }
	}
	return result;
}

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

	}

	Memory(const ArrayType& array) : array_{array} {}

	[[nodiscard]] auto direct_read(const uint16_t address) const
	{
		return array_[address];
	}

	[[nodiscard]] auto read(const uint16_t address) const
	{

		if (address <= 0x7fff) {
			return cartridge_.read(address);
		}

		if (address == 0xff00) {
			if (~direct_read(0xff00) & (1 << 4)) {
				return static_cast<uint8_t>(~(joypad_state_ & 0x0f));
			} else if (~direct_read(0xff00) & (1 << 5)) {
				return static_cast<uint8_t>(~((joypad_state_ >> 4) & 0x0f));
			}
		}
		return array_[address];
	}

	void direct_write(const uint16_t address, const uint8_t value)
	{
		array_[address] = value;
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

		else {
			array_[address] = value;
		}
	}

	[[nodiscard]] auto dump() const
	{
		return array_;
	}

	uint8_t joypad_state_ = {};
private:
	ArrayType array_ = {};
	Cartridge cartridge_ = {};
};

struct MakeMemory {
	std::vector<std::pair<uint16_t, uint8_t>> changes;

	auto get()
	{
		auto array = Memory::ArrayType{};
		for (const auto& [address, value] : changes) { array[address] = value; }
		return Memory{array};
	}
};

struct MemoryChanger {
	std::vector<std::pair<uint16_t, uint8_t>> changes;

	auto get(const Memory& orig_memory)
	{
		auto array = orig_memory.dump();

		for (const auto& [address, value] : changes) { array[address] = value; }
		return Memory{array};
	}
};

inline void applyChanges(Memory& memory, const std::vector<MemoryDiff>& changes)
{
	for (const auto [address, orig_value, new_value] : changes) { memory.write(address, new_value); }
}

inline auto revertChanges(const std::vector<MemoryDiff>& changes)
{
	auto reverted_changes = changes;
	for (auto& change : reverted_changes) { std::swap(change.orig_value, change.new_value); }
	return reverted_changes;
}

class MemorySnapshots {
public:
	MemorySnapshots(const Memory& memory) : memory_last_state_(memory) {}

	void add(const Memory& current_memory)
	{
		const auto diff = memory_diff(memory_last_state_, current_memory);
		diffs_.push_back(diff);
		updateMemory(diff);
	}

	Memory getMemory(const uint64_t steps) const
	{
		assert(steps <= diffs_.size());

		auto memory = memory_last_state_;

		for (auto reverse = rbegin(diffs_); reverse < rbegin(diffs_) + steps;++ reverse) {
			const auto reverted_changes = revertChanges(*reverse);
			applyChanges(memory, reverted_changes);
		}
		return memory;
	}

	auto getLastNonEmptyDiffs(const uint64_t count) {
		auto flattened = std::vector<MemoryDiff>{};
		for (const auto& snapshot_diffs : diffs_) {
			flattened.insert(cend(flattened), cbegin(snapshot_diffs), cend(snapshot_diffs));
		}
		const auto actuall_count = std::min(count, flattened.size());
		return std::vector(cend(flattened) - actuall_count, cend(flattened));
	}

private:
	void updateMemory(const std::vector<MemoryDiff>& diffs)
	{
		applyChanges(memory_last_state_, diffs);
	}

	Memory memory_last_state_;
	std::vector<std::vector<MemoryDiff>> diffs_;
};
