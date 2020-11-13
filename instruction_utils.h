#pragma once


struct Instruction {
	std::string mnemonic;
	uint16_t opcode;
	uint8_t size;
	std::function<uint8_t(Registers&, Memory&, const uint16_t&)> run;

	[[nodiscard]] auto operator()(Registers& regs, Memory& mem, const uint16_t& PC) const {
		const auto cycles = run(regs, mem, PC);
		const auto PC_new = regs.read("PC");
		regs.write("PC", PC_new + size);
		return cycles;
	}
};

// TODO: Merge (half) carries somehow?
// TODO: Are (half) carries correct?
// Detect half-carry for addition, see https://robdor.com/2016/08/10/gameboy-emulator-half-carry-flag/
[[nodiscard]] auto half_carry_add_8bit(const uint16_t a, const uint16_t b) {
	return (((a & 0xf) + (b & 0xf)) & 0x10) > 0;
}
[[nodiscard]] auto half_carry_add_16bit(const uint16_t a, const uint16_t b) {
	return (((a & 0x0fff) + (b & 0x0fff)) & 0x1000) > 0;
}

[[nodiscard]] auto carry_add_8bit(const uint16_t a, const uint16_t b) {
	return (((a & 0xff) + (b & 0xff)) & 0x100) > 0;
}
[[nodiscard]] auto carry_add_16bit(const uint16_t a, const uint16_t b) {
	return (((a & 0xffff) + (b & 0xffff)) & 0x10000) > 0;
}

// https://www.reddit.com/r/EmuDev/comments/4clh23/trouble_with_halfcarrycarry_flag/
[[nodiscard]] auto half_carry_sub_8bit(const uint16_t a, const uint16_t b) {
	return ((a & 0x0f) - (b & 0x0f)) < 0;
}

[[nodiscard]] auto half_carry_sub_16bit(const uint16_t a, const uint16_t b) {
	return ((a & 0x0fff) - (b & 0xfff)) < 0;
}

[[nodiscard]] auto carry_sub_8bit(const uint16_t a, const uint16_t b) {
	return ((a & 0xff) - (b & 0xff)) < 0;
}
[[nodiscard]] auto carry_sub_16bit(const uint16_t a, const uint16_t b) {
	return ((a & 0xffff) - (b & 0xffff)) < 0;
}

// TOOD: Cleanup these instruction_* functions

void instruction_rst(const uint8_t& value, Registers& regs, Memory& memory, const uint16_t& PC) {
	const auto PC_high = static_cast<uint8_t>((PC & 0xff00) >> 8);
	const auto PC_low = static_cast<uint8_t>(PC & 0x00ff);
	const auto SP = regs.read("SP");

	memory.write(SP - 1, PC_high);
	memory.write(SP - 2, PC_low);

	regs.write("SP", SP - 2);
	regs.write("PC", value - 1);
}

template<size_t kSize>
// TODO: remove _fn from name
void instruction_inc_fn(const char(&reg_name)[kSize], Registers& regs) {
	constexpr auto reg_name_size = kSize - 1; // Subtract \0 at the end
	// 8bit
	if constexpr (reg_name_size == 1) {
		const auto old_value = regs.read(reg_name);
		const auto new_value = static_cast<uint8_t>(old_value + 1);
		regs.write(reg_name, new_value);

		regs.set_flag("Z", new_value == 0x00);
		regs.set_flag("N", false);
		regs.set_flag("H", half_carry_add_8bit(old_value, 1));
	}
	// 16bit
	else {
		assert(false && "Not implemented yet");
	}
}

template<size_t kSize>
void instruction_dec_fn(const char(&reg_name)[kSize], Registers& regs) {
	constexpr auto reg_name_size = kSize - 1; // Subtract \0 at the end
	// 8bit
	if constexpr (reg_name_size == 1) {
		const auto old_value = regs.read(reg_name);
		const auto new_value = static_cast<uint8_t>(old_value - 1);
		regs.write(reg_name, new_value);

		regs.set_flag("Z", new_value == 0x00);
		regs.set_flag("N", true);
		regs.set_flag("H", half_carry_sub_8bit(old_value, 1));
	}
	// 16bit
	else {
		assert(false && "Not implemented yet");
	}
}


template<size_t kDestSize, size_t kSecondRegNameSize>
void instruction_add(const char(&dest_name)[kDestSize], const char(&second_reg_name)[kSecondRegNameSize], Registers& regs) {
	static_assert(kDestSize == kSecondRegNameSize, "Add for 8bit + 16bit or vice versa not implemented.");
	constexpr auto real_size = kDestSize - 1;

	// 8bit
	if constexpr (real_size == 1) {
		const auto second_reg = regs.read(second_reg_name);
		const auto dest_old = regs.read(dest_name);
		const auto dest_new = static_cast<decltype(second_reg)>(dest_old + second_reg);

		regs.write(dest_name, dest_new);

		regs.set_flag("Z", dest_new == 0); // TODO: This is the only difference between 8 and 16bit, merge rest?
		regs.set_flag("N", 0);
		regs.set_flag("H", half_carry_add_16bit(dest_old, second_reg));
		regs.set_flag("C", carry_add_16bit(dest_old, second_reg));
	}
	// 16bit
	else {
		const auto second_reg = regs.read(second_reg_name);
		const auto dest_old = regs.read(dest_name);
		const auto dest_new = static_cast<decltype(second_reg)>(dest_old + second_reg);

		regs.write(dest_name, dest_new);

		regs.set_flag("N", 0);
		regs.set_flag("H", half_carry_add_16bit(dest_old, second_reg));
		regs.set_flag("C", carry_add_16bit(dest_old, second_reg));
	}
}

// TODO: Unit unit test for this
template<size_t kDestSize, typename ValueType>
void instruction_addc(const char(&dest_name)[kDestSize], const ValueType& value, Registers& regs) {
	constexpr auto real_size = kDestSize - 1;
	static_assert(real_size == 1, "Only 8bit add with carry supported.");
	static_assert(std::is_same_v<ValueType, uint8_t>, "Only 8bit add with carry supported.");

	const auto C = regs.read_flag("C");
	const auto value_with_carry = static_cast<uint8_t>(value + C);
	auto half_carry = half_carry_add_8bit(value, C);
	auto carry = carry_add_8bit(value, C);

	const auto dest_old = regs.read(dest_name);
	const auto dest_new = static_cast<uint8_t>(dest_old + value_with_carry);
	half_carry |= half_carry_add_8bit(dest_old, value_with_carry);
	carry |= carry_add_8bit(dest_old, value_with_carry);

	regs.write(dest_name, dest_new);
	regs.set_flag("Z", dest_new == 0);
	regs.set_flag("N", false);
	regs.set_flag("H", half_carry);
	regs.set_flag("C", carry);
}

template<size_t kDestSize, size_t kSecondRegNameSize>
void instruction_addc(const char(&dest_name)[kDestSize], const char(&second_reg_name)[kSecondRegNameSize], Registers& regs) {
	instruction_addc(dest_name, regs.read(second_reg_name), regs);
}

// TODO: Unit unit test for this
template<size_t kDestSize, typename ValueType>
void instruction_sub(const char(&dest_name)[kDestSize], const ValueType& value, Registers& regs) {
	constexpr auto real_size = kDestSize - 1;
	static_assert(real_size == 1, "Only 8bit sub is supported.");
	static_assert(std::is_same_v<ValueType, uint8_t>, "Only 8bit sub is supported.");

	const auto dest_old = regs.read(dest_name);
	const auto dest_new = dest_old - value;

	regs.write(dest_name, dest_new);
	regs.set_flag("Z", dest_new == 0);
	regs.set_flag("N", true);
	regs.set_flag("H", half_carry_sub_8bit(dest_old, value));
	regs.set_flag("C", carry_sub_8bit(dest_old, value));
}

// TODO: Unit unit test for this
template<size_t kDestSize, size_t kSecondRegNameSize>
void instruction_sub(const char(&dest_name)[kDestSize], const char(&second_reg_name)[kSecondRegNameSize], Registers& regs) {
	instruction_sub(dest_name, regs.read(second_reg_name), regs);
}

// TODO: Unit unit test for this
template<size_t kDestSize, typename ValueType>
void instruction_subc(const char(&dest_name)[kDestSize], const ValueType& value, Registers& regs) {
	constexpr auto real_size = kDestSize - 1;
	static_assert(real_size == 1, "Only 8bit add with carry supported.");
	static_assert(std::is_same_v<ValueType, uint8_t>, "Only 8bit add with carry supported.");

	const auto C = regs.read_flag("C");
	const auto dest_old = regs.read(dest_name);

	const auto dest_old_minus_carry = static_cast<uint8_t>(dest_old - C);
	auto half_carry = half_carry_sub_8bit(dest_old, C);
	auto carry = carry_sub_8bit(dest_old, C);

	const auto dest_new = static_cast<uint8_t>(dest_old_minus_carry  - value);
	half_carry |= half_carry_sub_8bit(dest_old_minus_carry, value);
	carry |= carry_sub_8bit(dest_old_minus_carry, value);

	regs.write(dest_name, dest_new);
	regs.set_flag("Z", dest_new == 0);
	regs.set_flag("N", true);
	regs.set_flag("H", half_carry);
	regs.set_flag("C", carry);
}

// TODO: Unit unit test for this
template<size_t kDestSize, size_t kSecondRegNameSize>
void instruction_subc(const char(&dest_name)[kDestSize], const char(&second_reg_name)[kSecondRegNameSize], Registers& regs) {
	instruction_subc(dest_name, regs.read(second_reg_name), regs);
}


template<size_t kDestSize, typename ValueType>
void instruction_and(const char(&dest_name)[kDestSize], const ValueType& value, Registers& regs) {
	constexpr auto real_size = kDestSize - 1;
	static_assert(real_size == 1, "Only 8bit AND supported.");
	static_assert(std::is_same_v<ValueType, uint8_t>, "Only 8bit values supported.");

	const auto dest_old = regs.read(dest_name);
	const auto dest_new = static_cast<uint8_t>(dest_old & value);

	regs.write(dest_name, dest_new);
	regs.set_flag("Z", dest_new == 0);
	regs.set_flag("N", false);
	regs.set_flag("H", true);
	regs.set_flag("C", false);

}
template<size_t kDestSize, size_t kSecondRegNameSize>
void instruction_and(const char(&dest_name)[kDestSize], const char(&second_reg_name)[kSecondRegNameSize], Registers& regs) {
	static_assert(kDestSize == kSecondRegNameSize, "Registers must be of a same size. And only 8bit are supported.");
	instruction_and(dest_name, regs.read(second_reg_name), regs);
}

template<size_t kDestSize, typename ValueType>
void instruction_xor(const char(&dest_name)[kDestSize], const ValueType& value, Registers& regs) {
	constexpr auto real_size = kDestSize - 1;
	static_assert(real_size == 1, "Only 8bit XOR supported.");
	static_assert(std::is_same_v<ValueType, uint8_t>, "Only 8bit values supported.");

	const auto dest_old = regs.read(dest_name);
	const auto dest_new = static_cast<uint8_t>(dest_old ^ value);

	regs.write(dest_name, dest_new);
	regs.set_flag("Z", dest_new == 0);
	regs.set_flag("N", false);
	regs.set_flag("H", false);
	regs.set_flag("C", false);

}
template<size_t kDestSize, size_t kSecondRegNameSize>
void instruction_xor(const char(&dest_name)[kDestSize], const char(&second_reg_name)[kSecondRegNameSize], Registers& regs) {
	static_assert(kDestSize == kSecondRegNameSize, "Registers must be of a same size. And only 8bit are supported.");
	instruction_xor(dest_name, regs.read(second_reg_name), regs);
}

template<size_t kDestSize, typename ValueType>
void instruction_or(const char(&dest_name)[kDestSize], const ValueType& value, Registers& regs) {
	constexpr auto real_size = kDestSize - 1;
	static_assert(real_size == 1, "Only 8bit XOR supported.");
	static_assert(std::is_same_v<ValueType, uint8_t>, "Only 8bit values supported.");

	const auto dest_old = regs.read(dest_name);
	const auto dest_new = static_cast<uint8_t>(dest_old | value);

	regs.write(dest_name, dest_new);
	regs.set_flag("Z", dest_new == 0);
	regs.set_flag("N", false);
	regs.set_flag("H", false);
	regs.set_flag("C", false);

}
template<size_t kDestSize, size_t kSecondRegNameSize>
void instruction_or(const char(&dest_name)[kDestSize], const char(&second_reg_name)[kSecondRegNameSize], Registers& regs) {
	static_assert(kDestSize == kSecondRegNameSize, "Registers must be of a same size. And only 8bit are supported.");
	instruction_or(dest_name, regs.read(second_reg_name), regs);
}

template<size_t kDestSize, typename ValueType>
void instruction_cp(const char(&dest_name)[kDestSize], const ValueType& value, Registers& regs) {
	constexpr auto real_size = kDestSize - 1;
	static_assert(real_size == 1, "Only 8bit XOR supported.");
	static_assert(std::is_same_v<ValueType, uint8_t>, "Only 8bit values supported.");

	const auto dest_old = regs.read(dest_name);
	const auto dest_new = static_cast<uint8_t>(dest_old - value);

	// regs.write(dest_name, dest_new);
	regs.set_flag("Z", dest_new == 0);
	regs.set_flag("N", true);
	regs.set_flag("H", half_carry_sub_8bit(dest_old, value));
	regs.set_flag("C", carry_sub_8bit(dest_old, value));

}
template<size_t kDestSize, size_t kSecondRegNameSize>
void instruction_cp(const char(&dest_name)[kDestSize], const char(&second_reg_name)[kSecondRegNameSize], Registers& regs) {
	static_assert(kDestSize == kSecondRegNameSize, "Registers must be of a same size. And only 8bit are supported.");
	instruction_cp(dest_name, regs.read(second_reg_name), regs);
}

void instruction_rlc(const char (&reg_name)[2], Registers& regs) {
	const auto old_value = regs.read(reg_name);
	const auto carry = regs.read_flag("C");

	const auto new_value = static_cast<uint8_t>((old_value << 1) + carry);

	regs.write(reg_name, new_value);
	regs.set_flag("Z", new_value == 0);
	regs.set_flag("N", 0);
	regs.set_flag("H", 0);
	regs.set_flag("C", old_value & (1 << 7));
}
