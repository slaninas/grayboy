#pragma once

#include "catch2/catch.hpp"

#include <iostream>
#include <sstream>

#include "cpu.h"
#include "memory.h"

template<typename T>
void p(const T& cont, const size_t num_elements)
{
	std::cout << std::hex;
	for (size_t i = 0; i < std::min(cont.size(), num_elements); ++i) { std::cout << (int)cont[i] << '\n'; }
	std::cout << std::dec;
}

template<typename T>
void p(const T& cont)
{
	p(cont, cont.size());
}

class RegistersCompare : public Catch::MatcherBase<Registers> {
public:
	RegistersCompare(const Registers& registers) : registers_{registers} {}

	auto match(const Registers& other) const -> bool override
	{
		return other.dump() == registers_.dump();
	}

	auto describe() const -> std::string override
	{
		std::ostringstream ss;
		ss << " is equal to \n" << registers_;
		return ss.str();
	}

private:
	const Registers registers_;
};

template<size_t kSize>
auto getRandomArray()
{
	auto array = std::array<uint8_t, kSize>{};
	for (size_t i = 0; i < kSize; ++i) { array[i] = std::rand(); };
	return array;
}

auto getRandomRegisters()
{
	return Registers{getRandomArray<Registers::ArrayElementCount>()};
}

auto getRandomFlags()
{
	return static_cast<uint8_t>(std::rand());
}
