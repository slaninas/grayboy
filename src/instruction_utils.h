#pragma once

#include "memory.h"
#include "registers.h"

#include <functional>

struct Instruction {
	std::string mnemonic;
	uint16_t opcode;
	uint8_t size;
};

// Detect half-carry for addition, see https://robdor.com/2016/08/10/gameboy-emulator-half-carry-flag/
[[nodiscard]] inline auto half_carry_add_8bit(const uint16_t a, const uint16_t b)
{
	return (((a & 0xf) + (b & 0xf)) & 0x10) > 0;
}
[[nodiscard]] inline auto half_carry_add_16bit(const uint16_t a, const uint16_t b)
{
	return (((a & 0x0fff) + (b & 0x0fff)) & 0x1000) > 0;
}

[[nodiscard]] inline auto carry_add_8bit(const uint16_t a, const uint16_t b)
{
	return (((a & 0xff) + (b & 0xff)) & 0x100) > 0;
}
[[nodiscard]] inline auto carry_add_16bit(const uint16_t a, const uint16_t b)
{
	return (((a & 0xffff) + (b & 0xffff)) & 0x10000) > 0;
}

// https://www.reddit.com/r/EmuDev/comments/4clh23/trouble_with_halfcarrycarry_flag/
[[nodiscard]] inline auto half_carry_sub_8bit(const uint16_t a, const uint16_t b)
{
	return ((a & 0x0f) - (b & 0x0f)) < 0;
}

[[nodiscard]] inline auto half_carry_sub_16bit(const uint16_t a, const uint16_t b)
{
	return ((a & 0x0fff) - (b & 0xfff)) < 0;
}

[[nodiscard]] inline auto carry_sub_8bit(const uint16_t a, const uint16_t b)
{
	return ((a & 0xff) - (b & 0xff)) < 0;
}

[[nodiscard]] inline auto carry_sub_16bit(const uint16_t a, const uint16_t b)
{
	return ((a & 0xffff) - (b & 0xffff)) < 0;
}

inline void instruction_rst(const uint8_t& value, Registers& regs, Memory& memory, const uint16_t& PC)
{
	const auto PC_high = static_cast<uint8_t>(((PC + 1) & 0xff00) >> 8);
	const auto PC_low = static_cast<uint8_t>((PC + 1) & 0x00ff);
	const auto SP = regs.read("SP");

	memory.write(SP - 1, PC_high);
	memory.write(SP - 2, PC_low);

	regs.write("SP", SP - 2);
	regs.write("PC", value - 1);
}

template<size_t kSize>
void instruction_inc(const char (&reg_name)[kSize], Registers& regs)
{
	constexpr auto reg_name_size = kSize - 1; // Subtract \0 at the end
	static_assert(reg_name_size == 1);

	const auto old_value = regs.read(reg_name);
	const auto new_value = static_cast<uint8_t>(old_value + 1);
	regs.write(reg_name, new_value);

	regs.set_flag("Z", new_value == 0x00);
	regs.set_flag("N", false);
	regs.set_flag("H", half_carry_add_8bit(old_value, 1));
}

template<size_t kSize>
void instruction_dec(const char (&reg_name)[kSize], Registers& regs)
{
	constexpr auto reg_name_size = kSize - 1; // Subtract \0 at the end
	static_assert(reg_name_size == 1);

	const auto old_value = regs.read(reg_name);
	const auto new_value = static_cast<uint8_t>(old_value - 1);
	regs.write(reg_name, new_value);

	regs.set_flag("Z", new_value == 0x00);
	regs.set_flag("N", true);
	regs.set_flag("H", half_carry_sub_8bit(old_value, 1));
}

template<size_t kDestSize, size_t kSecondRegNameSize>
void instruction_add(
  const char (&dest_name)[kDestSize],
  const char (&second_reg_name)[kSecondRegNameSize],
  Registers& regs)
{
	static_assert(kDestSize == kSecondRegNameSize, "Add for 8bit + 16bit or vice versa not implemented.");
	constexpr auto real_size = kDestSize - 1;

	const auto second_reg = regs.read(second_reg_name);
	const auto dest_old = regs.read(dest_name);
	const auto dest_new = static_cast<decltype(second_reg)>(dest_old + second_reg);

	regs.write(dest_name, dest_new);
	regs.set_flag("N", false);

	if constexpr (real_size == 1 ) {
		regs.set_flag("Z", dest_new == 0);
		regs.set_flag("H", half_carry_add_8bit(dest_old, second_reg));
		regs.set_flag("C", carry_add_8bit(dest_old, second_reg));
	}
	else {
		regs.set_flag("H", half_carry_add_16bit(dest_old, second_reg));
		regs.set_flag("C", carry_add_16bit(dest_old, second_reg));
	}
}

template<size_t kDestSize, typename ValueType>
void instruction_addc(const char (&dest_name)[kDestSize], const ValueType& value, Registers& regs)
{
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
void instruction_addc(
  const char (&dest_name)[kDestSize],
  const char (&second_reg_name)[kSecondRegNameSize],
  Registers& regs)
{
	instruction_addc(dest_name, regs.read(second_reg_name), regs);
}

template<size_t kDestSize, typename ValueType>
void instruction_sub(const char (&dest_name)[kDestSize], const ValueType& value, Registers& regs)
{
	constexpr auto real_size = kDestSize - 1;
	static_assert(real_size == 1, "Only 8bit sub is supported.");
	static_assert(std::is_same_v<ValueType, uint8_t>, "Only 8bit sub is supported.");

	const auto dest_old = regs.read(dest_name);
	const auto dest_new = static_cast<uint8_t>(dest_old - value);

	regs.write(dest_name, dest_new);
	regs.set_flag("Z", dest_new == 0);
	regs.set_flag("N", true);
	regs.set_flag("H", half_carry_sub_8bit(dest_old, value));
	regs.set_flag("C", carry_sub_8bit(dest_old, value));
}

template<size_t kDestSize, size_t kSecondRegNameSize>
void instruction_sub(
  const char (&dest_name)[kDestSize],
  const char (&second_reg_name)[kSecondRegNameSize],
  Registers& regs)
{
	instruction_sub(dest_name, regs.read(second_reg_name), regs);
}

template<size_t kDestSize, typename ValueType>
void instruction_subc(const char (&dest_name)[kDestSize], const ValueType& value, Registers& regs)
{
	constexpr auto real_size = kDestSize - 1;
	static_assert(real_size == 1, "Only 8bit add with carry supported.");
	static_assert(std::is_same_v<ValueType, uint8_t>, "Only 8bit add with carry supported.");

	const auto C = regs.read_flag("C");
	const auto dest_old = regs.read(dest_name);

	const auto dest_old_minus_carry = static_cast<uint8_t>(dest_old - C);
	auto half_carry = half_carry_sub_8bit(dest_old, C);
	auto carry = carry_sub_8bit(dest_old, C);

	const auto dest_new = static_cast<uint8_t>(dest_old_minus_carry - value);
	half_carry |= half_carry_sub_8bit(dest_old_minus_carry, value);
	carry |= carry_sub_8bit(dest_old_minus_carry, value);

	regs.write(dest_name, dest_new);
	regs.set_flag("Z", dest_new == 0);
	regs.set_flag("N", true);
	regs.set_flag("H", half_carry);
	regs.set_flag("C", carry);
}

template<size_t kDestSize, size_t kSecondRegNameSize>
void instruction_subc(
  const char (&dest_name)[kDestSize],
  const char (&second_reg_name)[kSecondRegNameSize],
  Registers& regs)
{
	instruction_subc(dest_name, regs.read(second_reg_name), regs);
}

template<size_t kDestSize, typename ValueType>
void instruction_and(const char (&dest_name)[kDestSize], const ValueType& value, Registers& regs)
{
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
void instruction_and(
  const char (&dest_name)[kDestSize],
  const char (&second_reg_name)[kSecondRegNameSize],
  Registers& regs)
{
	static_assert(kDestSize == kSecondRegNameSize, "Registers must be of a same size. And only 8bit are supported.");
	instruction_and(dest_name, regs.read(second_reg_name), regs);
}

template<size_t kDestSize, typename ValueType>
void instruction_xor(const char (&dest_name)[kDestSize], const ValueType& value, Registers& regs)
{
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
void instruction_xor(
  const char (&dest_name)[kDestSize],
  const char (&second_reg_name)[kSecondRegNameSize],
  Registers& regs)
{
	static_assert(kDestSize == kSecondRegNameSize, "Registers must be of a same size. And only 8bit are supported.");
	instruction_xor(dest_name, regs.read(second_reg_name), regs);
}

template<size_t kDestSize, typename ValueType>
void instruction_or(const char (&dest_name)[kDestSize], const ValueType& value, Registers& regs)
{
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
void instruction_or(
  const char (&dest_name)[kDestSize],
  const char (&second_reg_name)[kSecondRegNameSize],
  Registers& regs)
{
	static_assert(kDestSize == kSecondRegNameSize, "Registers must be of a same size. And only 8bit are supported.");
	instruction_or(dest_name, regs.read(second_reg_name), regs);
}

template<size_t kDestSize, typename ValueType>
void instruction_cp(const char (&dest_name)[kDestSize], const ValueType& value, Registers& regs)
{
	constexpr auto real_size = kDestSize - 1;
	static_assert(real_size == 1, "Only 8bit XOR supported.");
	static_assert(std::is_same_v<ValueType, uint8_t>, "Only 8bit values supported.");

	const auto dest_old = regs.read(dest_name);
	const auto dest_new = static_cast<uint8_t>(dest_old - value);

	regs.set_flag("Z", dest_new == 0);
	regs.set_flag("N", true);
	regs.set_flag("H", half_carry_sub_8bit(dest_old, value));
	regs.set_flag("C", carry_sub_8bit(dest_old, value));
}

template<size_t kDestSize, size_t kSecondRegNameSize>
void instruction_cp(
  const char (&dest_name)[kDestSize],
  const char (&second_reg_name)[kSecondRegNameSize],
  Registers& regs)
{
	static_assert(kDestSize == kSecondRegNameSize, "Registers must be of a same size. And only 8bit are supported.");
	instruction_cp(dest_name, regs.read(second_reg_name), regs);
}

inline auto rlc(uint8_t old_value)
{
	const auto carry = static_cast<bool>(old_value & (1 << 7));
	const auto new_value = static_cast<uint8_t>(old_value << 1) + static_cast<uint8_t>(carry);
	return std::pair{new_value, carry};
}

inline void set_flags_for_rotate(Registers& regs, const uint8_t new_value, const bool carry)
{
	regs.set_flag("Z", new_value == 0);
	regs.set_flag("N", false);
	regs.set_flag("H", false);
	regs.set_flag("C", carry);
}

inline void set_flags_for_shift(Registers& regs, const uint8_t new_value, const bool carry)
{
	set_flags_for_rotate(regs, new_value, carry);
}

inline void instruction_rlc(const char (&reg_name)[2], Registers& regs)
{
	const auto old_value = regs.read(reg_name);
	const auto [new_value, carry] = rlc(old_value);
	regs.write(reg_name, new_value);
	set_flags_for_rotate(regs, new_value, carry);
}

inline auto rl(uint8_t old_value, const bool carry)
{
	const auto new_value = static_cast<uint8_t>(old_value << 1) + static_cast<uint8_t>(carry);
	const auto new_carry = static_cast<bool>(old_value & (1 << 7));
	return std::pair{new_value, new_carry};
}

inline void instruction_rl(const char (&reg_name)[2], Registers& regs)
{
	const auto old_value = regs.read(reg_name);
	const auto [new_value, carry] = rl(old_value, regs.read_flag("C"));
	regs.write(reg_name, new_value);
	set_flags_for_rotate(regs, new_value, carry);
}

inline auto rrc(uint8_t old_value)
{
	const auto carry = static_cast<bool>(old_value & 1);
	const auto new_value =
	  static_cast<uint8_t>(old_value >> 1) + static_cast<uint8_t>(static_cast<uint8_t>(carry) << 7);
	return std::pair{new_value, carry};
}

inline void instruction_rrc(const char (&reg_name)[2], Registers& regs)
{
	const auto old_value = regs.read(reg_name);
	const auto [new_value, carry] = rrc(old_value);
	regs.write(reg_name, new_value);
	set_flags_for_rotate(regs, new_value, carry);
}

inline auto rr(uint8_t old_value, const bool carry)
{
	const auto new_value =
	  static_cast<uint8_t>(old_value >> 1) + static_cast<uint8_t>(static_cast<uint8_t>(carry) << 7);
	const auto new_carry = static_cast<bool>(old_value & 1);
	return std::pair{new_value, new_carry};
}

inline void instruction_rr(const char (&reg_name)[2], Registers& regs)
{
	const auto old_value = regs.read(reg_name);
	const auto [new_value, carry] = rr(old_value, regs.read_flag("C"));
	regs.write(reg_name, new_value);
	set_flags_for_rotate(regs, new_value, carry);
}

inline auto sla(const uint8_t old_value)
{
	const auto new_value = static_cast<uint8_t>(old_value << 1);
	const auto new_carry = static_cast<bool>(old_value & (1 << 7));
	return std::pair{new_value, new_carry};
}

inline void instruction_sla(const char (&reg_name)[2], Registers& regs)
{
	const auto old_value = regs.read(reg_name);
	const auto [new_value, new_carry] = sla(old_value);
	regs.write(reg_name, new_value);
	set_flags_for_shift(regs, new_value, new_carry);
}

inline auto sra(const uint8_t old_value)
{
	const auto new_value = static_cast<uint8_t>((old_value >> 1) + (old_value & (1 << 7)));
	const auto new_carry = static_cast<bool>(old_value & 1);
	return std::pair{new_value, new_carry};
}

inline void instruction_sra(const char (&reg_name)[2], Registers& regs)
{
	const auto old_value = regs.read(reg_name);
	const auto [new_value, new_carry] = sra(old_value);
	regs.write(reg_name, new_value);
	set_flags_for_shift(regs, new_value, new_carry);
}

inline void set_flags_for_swap(Registers& regs, const uint8_t new_value)
{
	regs.set_flag("Z", new_value == 0);
	regs.set_flag("N", false);
	regs.set_flag("H", false);
	regs.set_flag("C", false);
}

inline auto swap(const uint8_t old_value)
{
	const auto lower_byte = static_cast<uint8_t>(old_value & 0x0f);
	const auto higher_byte = static_cast<uint8_t>((old_value & 0xf0) >> 4);
	return static_cast<uint8_t>((lower_byte << 4) + higher_byte);
}

inline void instruction_swap(const char (&reg_name)[2], Registers& regs)
{
	const auto old_value = regs.read(reg_name);
	const auto new_value = swap(old_value);
	regs.write(reg_name, new_value);
	set_flags_for_swap(regs, new_value);
}

inline auto srl(const uint8_t old_value)
{
	const auto new_value = static_cast<uint8_t>(old_value >> 1);
	const auto new_carry = static_cast<bool>(old_value & 1);
	return std::pair{new_value, new_carry};
}

inline void instruction_srl(const char (&reg_name)[2], Registers& regs)
{
	const auto old_value = regs.read(reg_name);
	const auto [new_value, new_carry] = srl(old_value);
	regs.write(reg_name, new_value);
	set_flags_for_shift(regs, new_value, new_carry);
}

inline auto bit(const uint8_t value, const uint8_t position, Registers& regs)
{
	const auto bit_value = static_cast<bool>(value & (1 << position));
	regs.set_flag("Z", !bit_value);
	regs.set_flag("N", false);
	regs.set_flag("H", true);
}

inline void instruction_bit(const char (&reg_name)[2], const uint8_t position, Registers& regs)
{
	const auto value = regs.read(reg_name);
	bit(value, position, regs);
}

inline auto set_bit(const uint8_t orig_value, const uint8_t position)
{
	assert(position <= 7);
	return static_cast<uint8_t>(orig_value | (1 << position));
}

inline void instruction_set_bit(const char (&reg_name)[2], const uint8_t position, Registers& regs)
{
	const auto new_value = set_bit(regs.read(reg_name), position);
	regs.write(reg_name, new_value);
}

inline auto instruction_set_bit_hl(Memory& memory, const Registers& regs, const uint8_t& bit) -> uint64_t {
	const auto HL = regs.read("HL");
	const auto old_value = memory.read(HL);

	const auto new_value = set_bit(old_value, bit);
	memory.write(HL, new_value);
	return 4;
}

inline auto reset_bit(const uint8_t orig_value, const uint8_t position)
{
	assert(position <= 7);
	const auto mask = static_cast<uint8_t>(0xff ^ (1 << position));
	return static_cast<uint8_t>(orig_value & mask);
}

inline void instruction_reset_bit(const char (&reg_name)[2], const uint8_t position, Registers& regs)
{
	const auto new_value = reset_bit(regs.read(reg_name), position);
	regs.write(reg_name, new_value);
}
