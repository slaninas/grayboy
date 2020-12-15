#pragma once

#include "catch2/catch.hpp"

#include <sstream>
#include <iostream>

// TODO: Add script that runs all tests given number of times with different --rng-seed

// TODO: Fix - don't use .../
#include "cpu.h"
#include "memory.h"

template<typename T>
void p(const T& cont, const size_t num_elements) {
	std::cout << std::hex;
	for (size_t i = 0; i < std::min(cont.size(), num_elements); ++i) {
		std::cout << (int)cont[i] << '\n';
	}
	std::cout << std::dec;
}


template<typename T>
void p(const T& cont) {
	p(cont, cont.size());
}

class RegistersCompare : public Catch::MatcherBase<Registers> {
public:
	RegistersCompare(const Registers& registers) :
		registers_{registers}
	{}

	virtual bool match(const Registers& other) const override {
		return other.dump() == registers_.dump();
	}

	virtual std::string describe() const override {
		std::ostringstream ss;
		// ss << " is equal to " << vector_;
		ss << " is equal to \n" << registers_;;
		return ss.str();
		// TODO: implmente
	}

private:
	const Registers registers_;
};

template<size_t kSize>
auto getRandomArray() {
	auto array = std::array<uint8_t, kSize>{};
	for (size_t i = 0; i < kSize; ++i) {
		array[i] = std::rand();
	};
	return array;
}

auto getRandomMemory() {
	return Memory{getRandomArray<Memory::ArrayElements>()};
}

auto getRandomRegisters() {
	return Registers{getRandomArray<Registers::ArrayElementCount>()};
}

auto getRandomFlags() {
	return static_cast<uint8_t>(std::rand());
}
