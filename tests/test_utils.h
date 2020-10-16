#pragma once

#include "catch2/catch.hpp"

#include<sstream>

#include "cpu.h"

class RegistersCompare : public Catch::MatcherBase<Registers> {
public:
	RegistersCompare(const Registers& registers) :
		registers_{registers}
	{}

	virtual bool match(const Registers& other) const override {
		return other.register_array == registers_.register_array;
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

