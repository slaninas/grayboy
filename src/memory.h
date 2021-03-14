#pragma once

#include <cstdint>
#include <array>
#include <cassert>
#include <vector>

// TODO: Move into utils file

struct MemoryDiff {
	uint16_t address;
	uint8_t orig_value;
	uint8_t new_value;
};

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

class Memory {
public:
	static const size_t ArrayElements = 1 << 16;
	using ArrayType = std::array<uint8_t, ArrayElements>;
	using AddressType = uint16_t;

	Memory() = default;
	Memory(const ArrayType& array) : array_{array} {}

	[[nodiscard]] auto read(const uint16_t address) const
	{
		return array_[address];
	}

	void write(const uint16_t address, const uint8_t value)
	{
		array_[address] = value;
	}

	[[nodiscard]] auto dump() const
	{
		return array_;
	}

private:
	ArrayType array_ = {};
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

private:
	void updateMemory(const std::vector<MemoryDiff>& diffs)
	{
		applyChanges(memory_last_state_, diffs);
	}

	Memory memory_last_state_;
	std::vector<std::vector<MemoryDiff>> diffs_;
};
