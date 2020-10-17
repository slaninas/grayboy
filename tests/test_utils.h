#pragma once

#include "catch2/catch.hpp"

#include<sstream>

#include "cpu.h"

template<typename T>
void p(const T& cont) {
	std::cout << std::hex;
	for (const auto& el : cont) {
		std::cout << (int)el << ' ';
	}
	std::cout << '\n';
	std::cout << std::dec;
}

class RegistersCompare : public Catch::MatcherBase<Registers> {
public:
	RegistersCompare(const Registers& registers) :
		registers_{registers}
	{}

	virtual bool match(const Registers& other) const override {
		return other.array_copy() == registers_.array_copy();
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

