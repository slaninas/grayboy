#pragma once

#include <array>
#include <vector>

class Memory {
public:
	static const size_t ArrayElements = 1 << 16;
	using ArrayType = std::array<uint8_t, ArrayElements>;
	using AddressType = uint16_t;

	Memory() = default;
	Memory(const ArrayType& array) : array_{array} {}

	[[nodiscard]] auto read(const uint16_t address) const {
		return array_[address];
	}

	void write(const uint16_t address, const uint8_t value) {
		array_[address] = value;
	}

	[[nodiscard]] auto dump() const {
		return array_;
	}

private:
	ArrayType array_ = {};
};

struct MakeMemory {
	std::vector<std::pair<uint16_t, uint8_t>> changes;

	auto get() {
		auto array = Memory::ArrayType{};
		for (const auto& [address, value] : changes) { array[address] = value; }
		return Memory{array};
	}
};

struct MemoryChanger {
	std::vector<std::pair<uint16_t, uint8_t>> changes;

	auto get(const Memory& orig_memory) {
		auto array = orig_memory.dump();

		for (const auto& [address, value] : changes) { array[address] = value; }
		return Memory{array};
	}
};
