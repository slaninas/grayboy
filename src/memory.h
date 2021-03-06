#pragma once

#include <array>
#include <vector>
#include <stack>
#include <cassert>

struct MemoryDiff {
	uint16_t address;
	uint8_t orig_value;
	uint8_t new_value;
};

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
		const auto orig_value = array_[address];
		array_[address] = value;
		changes_.push({address, orig_value, value});
	}

	[[nodiscard]] auto dump() const
	{
		return array_;
	}

	auto go_back(const uint64_t steps) -> void
	{
		assert(steps <= changes_.size());
		// TODO: Check if not going outside
		for (auto i = uint64_t{0}; i < steps; ++i) {
			const auto change = changes_.top();
			array_[change.address] = change.orig_value;
			changes_.pop();
		}

	}

private:
	ArrayType array_ = {};
	std::stack<MemoryDiff> changes_;
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
