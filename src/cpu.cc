#include "cpu.h"

#include <optional>

[[nodiscard]] auto find_by_opcode(const uint16_t opcode, const std::vector<Instruction>& instructions) -> Instruction
{
	const auto index = opcode <= 0xff ? opcode : (opcode & 0xff) + 0x100;
	return instructions[index];
}

[[nodiscard]] auto Cpu::execute_opcode(const uint16_t& opcode, const uint16_t& PC, Registers& regs, Memory& memory) -> uint8_t {
	switch (opcode) {
	case 0x00: {
		return 1;
		}

	case 0x01: {
		regs.write("C", memory.read(PC + 1));
		regs.write("B", memory.read(PC + 2));
		return 3;
		}

	case 0x11: {
		regs.write("E", memory.read(PC + 1));
		regs.write("D", memory.read(PC + 2));
		return 3;
		}

	case 0x21: {
		regs.write("L", memory.read(PC + 1));
		regs.write("H", memory.read(PC + 2));
		return 3;
		}

	case 0x31: {
		const auto value = static_cast<uint16_t>(memory.read(PC + 1) + (memory.read(PC + 2) << 8));
		regs.write("SP", value);
		return 3;
		}

	case 0x02: {
		memory.write(regs.read("BC"), regs.read("A"));
		return 2;
		}

	case 0x12: {
		memory.write(regs.read("DE"), regs.read("A"));
		return 2;
		}

	case 0x22: {
		memory.write(regs.read("HL"), regs.read("A"));
		regs.write("HL", regs.read("HL") + 1);
		return 2;
		}

	case 0x32: {
		memory.write(regs.read("HL"), regs.read("A"));
		regs.write("HL", regs.read("HL") - 1);
		return 2;
		}

	case 0x03: {
		regs.write("BC", regs.read("BC") + 1);
		return 2;
		}

	case 0x13: {
		regs.write("DE", regs.read("DE") + 1);
		return 2;
		}

	case 0x23: {
		regs.write("HL", regs.read("HL") + 1);
		return 2;
		}

	case 0x33: {
		regs.write("SP", regs.read("SP") + 1);
		return 2;
		}

	case 0x04: {
		instruction_inc_fn("B", regs);
		return 1;
		}

	case 0x0c: {
		instruction_inc_fn("C", regs);
		return 1;
		}

	case 0x1c: {
		instruction_inc_fn("E", regs);
		return 1;
		}

	case 0x2c: {
		instruction_inc_fn("L", regs);
		return 1;
		}

	case 0x3c: {
		instruction_inc_fn("A", regs);
		return 1;
		}

	case 0x14: {
		instruction_inc_fn("D", regs);
		return 1;
		}

	case 0x24: {
		instruction_inc_fn("H", regs);
		return 1;
		}

	case 0x34: {
		// TODO: Use instruction_inc method?
		const auto address = regs.read("HL");
		const auto old_value = memory.read(address);
		const auto new_value = static_cast<uint8_t>(old_value + 1);
		memory.write(address, new_value);
		
		regs.set_flag("Z", new_value == 0x00);
		regs.set_flag("N", false);
		regs.set_flag("H", half_carry_add_8bit(old_value, 1));
		return 3;
		}

	case 0x05: {
		instruction_dec_fn("B", regs);
		return 1;
		}

	case 0x0d: {
		instruction_dec_fn("C", regs);
		return 1;
		}

	case 0x1d: {
		instruction_dec_fn("E", regs);
		return 1;
		}

	case 0x2d: {
		instruction_dec_fn("L", regs);
		return 1;
		}

	case 0x3d: {
		instruction_dec_fn("A", regs);
		return 1;
		}

	case 0x15: {
		instruction_dec_fn("D", regs);
		return 1;
		}

	case 0x25: {
		instruction_dec_fn("H", regs);
		return 1;
		}

	case 0x35: {
		// TODO: Use instruction_dec function?
		const auto address = regs.read("HL");
		const auto old_value = memory.read(address);
		const auto new_value = static_cast<uint8_t>(old_value - 1);
		memory.write(address, new_value);
		
		regs.set_flag("Z", new_value == 0x00);
		regs.set_flag("N", true);
		regs.set_flag("H", half_carry_sub_8bit(old_value, 1));
		return 3;
		}

	case 0x06: {
		regs.write("B", memory.read(PC + 1));
		return 2;
		}

	case 0x16: {
		regs.write("D", memory.read(PC + 1));
		return 2;
		}

	case 0x26: {
		regs.write("H", memory.read(PC + 1));
		return 2;
		}

	case 0x36: {
		const auto address = regs.read("HL");
		const auto value = memory.read(PC + 1);
		memory.write(address, value);
		return 3;
		}

	case 0x0a: {
		const auto address = regs.read("BC");
		regs.write("A", memory.read(address));
		return 2;
		}

	case 0x1a: {
		const auto address = regs.read("DE");
		regs.write("A", memory.read(address));
		return 2;
		}

	case 0x2a: {
		const auto address = regs.read("HL");
		regs.write("A", memory.read(address));
		regs.write("HL", address + 1);
		return 2;
		}

	case 0x3a: {
		const auto address = regs.read("HL");
		regs.write("A", memory.read(address));
		regs.write("HL", address - 1);
		return 2;
		}

	case 0x0e: {
		const auto value = memory.read(PC + 1);
		regs.write("C", value);
		return 2;
		}

	case 0x1e: {
		const auto value = memory.read(PC + 1);
		regs.write("E", value);
		return 2;
		}

	case 0x2e: {
		const auto value = memory.read(PC + 1);
		regs.write("L", value);
		return 2;
		}

	case 0x3e: {
		const auto value = memory.read(PC + 1);
		regs.write("A", value);
		return 2;
		}

	case 0x40: {
		return 1;
		}

	case 0x41: {
		regs.write("B", regs.read("C"));
		return 1;
		}

	case 0x42: {
		regs.write("B", regs.read("D"));
		return 1;
		}

	case 0x43: {
		regs.write("B", regs.read("E"));
		return 1;
		}

	case 0x44: {
		regs.write("B", regs.read("H"));
		return 1;
		}

	case 0x45: {
		regs.write("B", regs.read("L"));
		return 1;
		}

	case 0x46: {
		const auto address = regs.read("HL");
		regs.write("B", memory.read(address));
		return 2;
		}

	case 0x47: {
		regs.write("B", regs.read("A"));
		return 1;
		}

	case 0x48: {
		regs.write("C", regs.read("B"));
		return 1;
		}

	case 0x49: {
		return 1;
		}

	case 0x4a: {
		regs.write("C", regs.read("D"));
		return 1;
		}

	case 0x4b: {
		regs.write("C", regs.read("E"));
		return 1;
		}

	case 0x4c: {
		regs.write("C", regs.read("H"));
		return 1;
		}

	case 0x4d: {
		regs.write("C", regs.read("L"));
		return 1;
		}

	case 0x4e: {
		const auto address = regs.read("HL");
		regs.write("C", memory.read(address));
		return 2;
		}

	case 0x4f: {
		regs.write("C", regs.read("A"));
		return 1;
		}

	case 0x50: {
		regs.write("D", regs.read("B"));
		return 1;
		}

	case 0x51: {
		regs.write("D", regs.read("C"));
		return 1;
		}

	case 0x52: {
		return 1;
		}

	case 0x53: {
		regs.write("D", regs.read("E"));
		return 1;
		}

	case 0x54: {
		regs.write("D", regs.read("H"));
		return 1;
		}

	case 0x55: {
		regs.write("D", regs.read("L"));
		return 1;
		}

	case 0x56: {
		const auto address = regs.read("HL");
		regs.write("D", memory.read(address));
		return 2;
		}

	case 0x57: {
		regs.write("D", regs.read("A"));
		return 1;
		}

	case 0x58: {
		regs.write("E", regs.read("B"));
		return 1;
		}

	case 0x59: {
		regs.write("E", regs.read("C"));
		return 1;
		}

	case 0x5a: {
		regs.write("E", regs.read("D"));
		return 1;
		}

	case 0x5b: {
		return 1;
		}

	case 0x5c: {
		regs.write("E", regs.read("H"));
		return 1;
		}

	case 0x5d: {
		regs.write("E", regs.read("L"));
		return 1;
		}

	case 0x5e: {
		const auto address = regs.read("HL");
		regs.write("E", memory.read(address));
		return 2;
		}

	case 0x5f: {
		regs.write("E", regs.read("A"));
		return 1;
		}

	case 0x60: {
		regs.write("H", regs.read("B"));
		return 1;
		}

	case 0x61: {
		regs.write("H", regs.read("C"));
		return 1;
		}

	case 0x62: {
		regs.write("H", regs.read("D"));
		return 1;
		}

	case 0x63: {
		regs.write("H", regs.read("E"));
		return 1;
		}

	case 0x64: {
		return 1;
		}

	case 0x65: {
		regs.write("H", regs.read("L"));
		return 1;
		}

	case 0x66: {
		const auto address = regs.read("HL");
		regs.write("H", memory.read(address));
		return 2;
		}

	case 0x67: {
		regs.write("H", regs.read("A"));
		return 1;
		}

	case 0x68: {
		regs.write("L", regs.read("B"));
		return 1;
		}

	case 0x69: {
		regs.write("L", regs.read("C"));
		return 1;
		}

	case 0x6a: {
		regs.write("L", regs.read("D"));
		return 1;
		}

	case 0x6b: {
		regs.write("L", regs.read("E"));
		return 1;
		}

	case 0x6c: {
		regs.write("L", regs.read("H"));
		return 1;
		}

	case 0x6d: {
		return 1;
		}

	case 0x6e: {
		const auto address = regs.read("HL");
		regs.write("L", memory.read(address));
		return 2;
		}

	case 0x6f: {
		regs.write("L", regs.read("A"));
		return 1;
		}

	case 0x70: {
		const auto address = regs.read("HL");
		const auto value = regs.read("B");
		memory.write(address, value);
		return 2;
		}

	case 0x71: {
		const auto address = regs.read("HL");
		const auto value = regs.read("C");
		memory.write(address, value);
		return 2;
		}

	case 0x72: {
		const auto address = regs.read("HL");
		const auto value = regs.read("D");
		memory.write(address, value);
		return 2;
		}

	case 0x73: {
		const auto address = regs.read("HL");
		const auto value = regs.read("E");
		memory.write(address, value);
		return 2;
		}

	case 0x74: {
		const auto address = regs.read("HL");
		const auto value = regs.read("H");
		memory.write(address, value);
		return 2;
		}

	case 0x75: {
		const auto address = regs.read("HL");
		const auto value = regs.read("L");
		memory.write(address, value);
		return 2;
		}

	case 0x77: {
		const auto address = regs.read("HL");
		const auto value = regs.read("A");
		memory.write(address, value);
		return 2;
		}

	case 0x78: {
		regs.write("A", regs.read("B"));
		return 1;
		}

	case 0x79: {
		regs.write("A", regs.read("C"));
		return 1;
		}

	case 0x7a: {
		regs.write("A", regs.read("D"));
		return 1;
		}

	case 0x7b: {
		regs.write("A", regs.read("E"));
		return 1;
		}

	case 0x7c: {
		regs.write("A", regs.read("H"));
		return 1;
		}

	case 0x7d: {
		regs.write("A", regs.read("L"));
		return 1;
		}

	case 0x7e: {
		const auto address = regs.read("HL");
		const auto value = memory.read(address);
		regs.write("A", value);
		return 2;
		}

	case 0x7f: {
		return 1;
		}

	case 0xe0: {
		const auto address = static_cast<uint16_t>(0xff00 + memory.read(PC + 1));
		const auto value = regs.read("A");
		memory.write(address, value);
		return 3;
		}

	case 0xf0: {
		const auto address = static_cast<uint16_t>(0xff00 + memory.read(PC + 1));
		const auto value = memory.read(address);
		regs.write("A", value);
		return 3;
		}

	case 0xe2: {
		const auto address = static_cast<uint16_t>(0xff00 + regs.read("C"));
		const auto value = regs.read("A");
		memory.write(address, value);
		return 2;
		}

	case 0xf2: {
		const auto address = static_cast<uint16_t>(0xff00 + regs.read("C"));
		const auto value = memory.read(address);
		regs.write("A", value);
		return 2;
		}

	case 0xc1: {
		const auto SP = regs.read("SP");
		const auto BC_new = static_cast<uint16_t>((memory.read(SP + 1) << 8) + memory.read(SP));
		regs.write("BC", BC_new);
		regs.write("SP", SP + 2);
		return 3;
		}

	case 0xd1: {
		const auto SP = regs.read("SP");
		const auto DE_new = static_cast<uint16_t>((memory.read(SP + 1) << 8) + memory.read(SP));
		regs.write("DE", DE_new);
		regs.write("SP", SP + 2);
		return 3;
		}

	case 0xe1: {
		const auto SP = regs.read("SP");
		const auto HL_new = static_cast<uint16_t>((memory.read(SP + 1) << 8) + memory.read(SP));
		regs.write("HL", HL_new);
		regs.write("SP", SP + 2);
		return 3;
		}

	case 0xf1: {
		const auto SP = regs.read("SP");
		const auto AF_new = static_cast<uint16_t>((memory.read(SP + 1) << 8) + memory.read(SP));
		regs.write("AF", AF_new);
		regs.write("SP", SP + 2);
		return 3;
		}

	case 0xc5: {
		const auto SP = regs.read("SP");
		memory.write(SP - 1, regs.read("B"));
		memory.write(SP - 2, regs.read("C"));
		regs.write("SP", SP - 2);
		return 4;
		}

	case 0xd5: {
		const auto SP = regs.read("SP");
		memory.write(SP - 1, regs.read("D"));
		memory.write(SP - 2, regs.read("E"));
		regs.write("SP", SP - 2);
		return 4;
		}

	case 0xe5: {
		const auto SP = regs.read("SP");
		memory.write(SP - 1, regs.read("H"));
		memory.write(SP - 2, regs.read("L"));
		regs.write("SP", SP - 2);
		return 4;
		}

	case 0xf5: {
		const auto SP = regs.read("SP");
		memory.write(SP - 1, regs.read("A"));
		memory.write(SP - 2, regs.read("F"));
		regs.write("SP", SP - 2);
		return 4;
		}

	case 0xf8: {
		const auto value = static_cast<int8_t>(memory.read(PC + 1));
		const auto SP = regs.read("SP");
		
		regs.write("HL", static_cast<uint16_t>(SP + value));
		regs.set_flag("Z", false);
		regs.set_flag("N", false);
		regs.set_flag("H", half_carry_add_8bit(SP, value));
		regs.set_flag("C", carry_add_8bit(SP, value));
		
		return 3;
		}

	case 0xf9: {
		regs.write("SP", regs.read("HL"));
		return 2;
		}

	case 0x0b: {
		regs.write("BC", regs.read("BC") - 1);
		return 2;
		}

	case 0x1b: {
		regs.write("DE", regs.read("DE") - 1);
		return 2;
		}

	case 0x2b: {
		regs.write("HL", regs.read("HL") - 1);
		return 2;
		}

	case 0x3b: {
		regs.write("SP", regs.read("SP") - 1);
		return 2;
		}

	case 0x07: {
		// TODO: Put into method
		const auto A = regs.read("A");
		const auto msb = (A & (1 << 7)) >> 7;
		const auto A_new = static_cast<uint8_t>((A << 1) + msb);
		regs.write("A", A_new);
		
		regs.set_flag("Z", false);
		regs.set_flag("N", false);
		regs.set_flag("H", false);
		regs.set_flag("C", static_cast<bool>(msb));
		return 1;
		}

	case 0x17: {
		// TODO: Put into method
		const auto A = regs.read("A");
		const auto msb = (A & (1 << 7)) >> 7;
		const auto A_new = static_cast<uint8_t>((A << 1) + regs.read_flag("C")); // Setting C here as bit 0, it's only difference from RLCA which uses msb
		regs.write("A", A_new);
		
		regs.set_flag("Z", false);
		regs.set_flag("N", false);
		regs.set_flag("H", false);
		regs.set_flag("C", static_cast<bool>(msb));
		return 1;
		}

	case 0x0f: {
		// TODO: Put into method
		const auto A = regs.read("A");
		const auto lsb = (A & (1 << 0)) >> 0;
		const auto A_new = static_cast<uint8_t>((A >> 1) + (lsb << 7));
		regs.write("A", A_new);
		
		regs.set_flag("Z", false);
		regs.set_flag("N", false);
		regs.set_flag("H", false);
		regs.set_flag("C", static_cast<bool>(lsb));
		return 1;
		}

	case 0x1f: {
		// TODO: Put into method
		const auto A = regs.read("A");
		const auto lsb = (A & (1 << 0)) >> 0;
		const auto A_new = static_cast<uint8_t>((A >> 1) + (regs.read_flag("C") << 7)); // Setting C here as bit 0, it's only difference from RLCA which uses msb
		regs.write("A", A_new);
		
		regs.set_flag("Z", false);
		regs.set_flag("N", false);
		regs.set_flag("H", false);
		regs.set_flag("C", static_cast<bool>(lsb));
		return 1;
		}

	case 0x08: {
		// TODO: Put into method
		const auto address = static_cast<uint16_t>((memory.read(PC + 2) << 8) + memory.read(PC + 1));
		const auto SP = regs.read("SP");
		memory.write(address, static_cast<uint8_t>(SP & 0x00ff));
		memory.write(address + 1, static_cast<uint8_t>((SP & 0xff00) >> 8));
		return 5;
		}

	case 0xea: {
		// TODO: Check byte order
		const auto address = static_cast<uint16_t>((memory.read(PC + 2) << 8) + memory.read(PC + 1));
		const auto value = regs.read("A");
		memory.write(address, value);
		return 4;
		}

	case 0xfa: {
		const auto address = static_cast<uint16_t>((memory.read(PC + 2) << 8) + memory.read(PC + 1));
		const auto value = memory.read(address);
		regs.write("A", value);
		return 4;
		}

	case 0x27: {
		// https://forums.nesdev.org/viewtopic.php?p=196282&sid=3441048d6a2d28d493f69044754e4e42#p196282
		const auto A = regs.read("A");
		
		const auto N = regs.read_flag("N");
		const auto H = regs.read_flag("H");
		const auto C = regs.read_flag("C");
		
		
		auto A_new = A;
		auto C_new = C;
		
		if (!N) {
		if (C_new || A_new > 0x99) {
		A_new += 0x60;
		C_new = true;
		}
		if (H || (A_new & 0xf) > 0x09)  {
		A_new += 0x6;
		}
		} else {
		if (C_new) {
		A_new -= 0x60;
		}
		if (H) {
		A_new -= 0x6;
		}
		}
		
		regs.write("A", A_new);
		regs.set_flag("Z", A_new == 0);
		regs.set_flag("H", 0);
		regs.set_flag("C", C_new);
		
		
		return 1;
		}

	case 0x37: {
		regs.set_flag("N", false);
		regs.set_flag("H", false);
		regs.set_flag("C", true);
		return 1;
		}

	case 0x76: {
		regs.set_halt(true);
		return 1;
		}

	case 0x2f: {
		const auto A = regs.read("A");
		const auto A_new = A ^ 0xff;
		regs.write("A", A_new);
		regs.set_flag("N", true);
		regs.set_flag("H", true);
		return 1;
		}

	case 0x3f: {
		regs.set_flag("N", false);
		regs.set_flag("H", false);
		regs.set_flag("C", !regs.read_flag("C"));
		return 1;
		}

	case 0xf3: {
		regs.set_IME(false);
		return 1;
		}

	case 0xfb: {
		regs.set_IME(true);
		return 1;
		}

	case 0x09: {
		instruction_add("HL", "BC", regs);
		return 2;
		}

	case 0x19: {
		instruction_add("HL", "DE", regs);
		return 2;
		}

	case 0x29: {
		instruction_add("HL", "HL", regs);
		return 2;
		}

	case 0x39: {
		instruction_add("HL", "SP", regs);
		return 2;
		}

	case 0x80: {
		instruction_add("A", "B", regs);
		return 1;
		}

	case 0x81: {
		instruction_add("A", "C", regs);
		return 1;
		}

	case 0x82: {
		instruction_add("A", "D", regs);
		return 1;
		}

	case 0x83: {
		instruction_add("A", "E", regs);
		return 1;
		}

	case 0x84: {
		instruction_add("A", "H", regs);
		return 1;
		}

	case 0x85: {
		instruction_add("A", "L", regs);
		return 1;
		}

	case 0x86: {
		// TODO: Put into function
		const auto address = regs.read("HL");
		const auto value = memory.read(address);
		const auto A_old = regs.read("A");
		const auto A_new = static_cast<uint8_t>(A_old + value);
		
		regs.write("A", A_new);
		regs.set_flag("Z", A_new == 0);
		regs.set_flag("N", false);
		regs.set_flag("H", half_carry_add_8bit(A_old, value));
		regs.set_flag("C", carry_add_8bit(A_old, value));
		return 2;
		}

	case 0x87: {
		instruction_add("A", "A", regs);
		return 1;
		}

	case 0x88: {
		instruction_addc("A", "B", regs);
		return 1;
		}

	case 0x89: {
		instruction_addc("A", "C", regs);
		return 1;
		}

	case 0x8a: {
		instruction_addc("A", "D", regs);
		return 1;
		}

	case 0x8b: {
		instruction_addc("A", "E", regs);
		return 1;
		}

	case 0x8c: {
		instruction_addc("A", "H", regs);
		return 1;
		}

	case 0x8d: {
		instruction_addc("A", "L", regs);
		return 1;
		}

	case 0x8e: {
		const auto address = regs.read("HL");
		const auto value = memory.read(address);
		instruction_addc("A", value, regs);
		return 2;
		}

	case 0x8f: {
		instruction_addc("A", "A", regs);
		return 1;
		}

	case 0xce: {
		const auto value = memory.read(PC + 1);
		instruction_addc("A", value, regs);
		return 2;
		}

	case 0x90: {
		instruction_sub("A", "B", regs);
		return 1;
		}

	case 0x91: {
		instruction_sub("A", "C", regs);
		return 1;
		}

	case 0x92: {
		instruction_sub("A", "D", regs);
		return 1;
		}

	case 0x93: {
		instruction_sub("A", "E", regs);
		return 1;
		}

	case 0x94: {
		instruction_sub("A", "H", regs);
		return 1;
		}

	case 0x95: {
		instruction_sub("A", "L", regs);
		return 1;
		}

	case 0x96: {
		const auto address = regs.read("HL");
		const auto value = memory.read(address);
		instruction_sub("A", value, regs);
		return 2;
		}

	case 0x97: {
		instruction_sub("A", "A", regs);
		return 1;
		}

	case 0x98: {
		instruction_subc("A", "B", regs);
		return 1;
		}

	case 0x99: {
		instruction_subc("A", "C", regs);
		return 1;
		}

	case 0x9a: {
		instruction_subc("A", "D", regs);
		return 1;
		}

	case 0x9b: {
		instruction_subc("A", "E", regs);
		return 1;
		}

	case 0x9c: {
		instruction_subc("A", "H", regs);
		return 1;
		}

	case 0x9d: {
		instruction_subc("A", "L", regs);
		return 1;
		}

	case 0x9e: {
		const auto address = regs.read("HL");
		const auto value = memory.read(address);
		instruction_subc("A", value, regs);
		return 2;
		}

	case 0x9f: {
		instruction_subc("A", "A", regs);
		return 1;
		}

	case 0xde: {
		const auto value = memory.read(PC + 1);
		instruction_subc("A", value, regs);
		return 2;
		}

	case 0xa0: {
		instruction_and("A", "B", regs);
		return 1;
		}

	case 0xa1: {
		instruction_and("A", "C", regs);
		return 1;
		}

	case 0xa2: {
		instruction_and("A", "D", regs);
		return 1;
		}

	case 0xa3: {
		instruction_and("A", "E", regs);
		return 1;
		}

	case 0xa4: {
		instruction_and("A", "H", regs);
		return 1;
		}

	case 0xa5: {
		instruction_and("A", "L", regs);
		return 1;
		}

	case 0xa6: {
		const auto address = regs.read("HL");
		const auto value = memory.read(address);
		instruction_and("A", value, regs);
		return 2;
		}

	case 0xa7: {
		instruction_and("A", "A", regs);
		return 1;
		}

	case 0xa8: {
		instruction_xor("A", "B", regs);
		return 1;
		}

	case 0xa9: {
		instruction_xor("A", "C", regs);
		return 1;
		}

	case 0xaa: {
		instruction_xor("A", "D", regs);
		return 1;
		}

	case 0xab: {
		instruction_xor("A", "E", regs);
		return 1;
		}

	case 0xac: {
		instruction_xor("A", "H", regs);
		return 1;
		}

	case 0xad: {
		instruction_xor("A", "L", regs);
		return 1;
		}

	case 0xae: {
		const auto address = regs.read("HL");
		const auto value = memory.read(address);
		instruction_xor("A", value, regs);
		return 2;
		}

	case 0xaf: {
		instruction_xor("A", "A", regs);
		return 1;
		}

	case 0xee: {
		const auto value = memory.read(PC + 1);
		instruction_xor("A", value, regs);
		return 2;
		}

	case 0xb0: {
		instruction_or("A", "B", regs);
		return 1;
		}

	case 0xb1: {
		instruction_or("A", "C", regs);
		return 1;
		}

	case 0xb2: {
		instruction_or("A", "D", regs);
		return 1;
		}

	case 0xb3: {
		instruction_or("A", "E", regs);
		return 1;
		}

	case 0xb4: {
		instruction_or("A", "H", regs);
		return 1;
		}

	case 0xb5: {
		instruction_or("A", "L", regs);
		return 1;
		}

	case 0xb6: {
		const auto address = regs.read("HL");
		const auto value = memory.read(address);
		instruction_or("A", value, regs);
		return 2;
		}

	case 0xb7: {
		instruction_or("A", "A", regs);
		return 1;
		}

	case 0xb8: {
		instruction_cp("A", "B", regs);
		return 1;
		}

	case 0xb9: {
		instruction_cp("A", "C", regs);
		return 1;
		}

	case 0xba: {
		instruction_cp("A", "D", regs);
		return 1;
		}

	case 0xbb: {
		instruction_cp("A", "E", regs);
		return 1;
		}

	case 0xbc: {
		instruction_cp("A", "H", regs);
		return 1;
		}

	case 0xbd: {
		instruction_cp("A", "L", regs);
		return 1;
		}

	case 0xbe: {
		const auto address = regs.read("HL");
		const auto value = memory.read(address);
		instruction_cp("A", value, regs);
		return 2;
		}

	case 0xbf: {
		instruction_cp("A", "A", regs);
		return 1;
		}

	case 0xfe: {
		const auto value = memory.read(PC + 1);
		const auto A = regs.read("A");
		const auto result = A - value;
		
		regs.set_flag("Z", result == 0);
		regs.set_flag("N", true);
		regs.set_flag("H", half_carry_sub_8bit(A, value));
		regs.set_flag("C", carry_sub_8bit(A, value));
		return 2;
		}

	case 0xc6: {
		const auto value = memory.read(PC + 1);
		const auto A_old = regs.read("A");
		const auto A_new = static_cast<uint8_t>(A_old + value);
		
		regs.write("A", A_new);
		regs.set_flag("Z", A_new == 0);
		regs.set_flag("N", false);
		regs.set_flag("H", half_carry_add_8bit(A_old, value));
		regs.set_flag("C", carry_add_8bit(A_old, value));
		return 2;
		}

	case 0xd6: {
		const auto value = memory.read(PC + 1);
		const auto A_old = regs.read("A");
		const auto A_new = static_cast<uint8_t>(A_old - value);
		
		regs.write("A", A_new);
		regs.set_flag("Z", A_new == 0);
		regs.set_flag("N", true);
		regs.set_flag("H", half_carry_sub_8bit(A_old, value));
		regs.set_flag("C", carry_sub_8bit(A_old, value));
		return 2;
		}

	case 0xe6: {
		const auto value = memory.read(PC + 1);
		instruction_and("A", value, regs);
		return 2;
		}

	case 0xf6: {
		const auto value = memory.read(PC + 1);
		instruction_or("A", value, regs);
		return 2;
		}

	case 0xe8: {
		const auto value = static_cast<int8_t>(memory.read(PC + 1));
		const auto SP_old = regs.read("SP");
		const auto SP_new = static_cast<uint16_t>(SP_old + value);
		
		regs.write("SP", SP_new);
		regs.set_flag("Z", false);
		regs.set_flag("N", false);
		regs.set_flag("H", half_carry_add_8bit(SP_old, value));
		regs.set_flag("C", carry_add_8bit(SP_old, value));
		return 4;
		}

	case 0x18: {
		const auto value = static_cast<int8_t>(memory.read(PC + 1));
		const auto PC_new = static_cast<uint16_t>(PC + value);
		// const auto PC_new = PC + value - 2 + 2; // Instruction size is 2 so it must be subtracted here in advance for Instruction to work properly
		regs.write("PC", PC_new - 2 + 2);
		return 3;
		}

	case 0x28: {
		if (regs.read_flag("Z")) {
		const auto value = static_cast<int8_t>(memory.read(PC + 1));
		const auto PC_new = static_cast<uint16_t>(PC + value);
		regs.write("PC", PC_new - 2 + 2);
		return 3;
		}
		return 2;
		}

	case 0x38: {
		if (regs.read_flag("C")) {
		const auto value = static_cast<int8_t>(memory.read(PC + 1));
		const auto PC_new = static_cast<uint16_t>(PC + value);
		regs.write("PC", PC_new - 2 + 2);
		return 3;
		}
		return 2;
		}

	case 0xc0: {
		if (!regs.read_flag("Z")) {
		const auto SP = regs.read("SP");
		const auto PC_new = static_cast<uint16_t>((memory.read(SP + 1) << 8) + memory.read(SP));
		regs.write("PC", PC_new - 1);
		regs.write("SP", SP + 2);
		return 5;
		}
		return 2;
		}

	case 0xd0: {
		if (!regs.read_flag("C")) {
		const auto SP = regs.read("SP");
		const auto PC_new = static_cast<uint16_t>((memory.read(SP + 1) << 8) + memory.read(SP));
		regs.write("PC", PC_new - 1);
		regs.write("SP", SP + 2);
		return 5;
		}
		return 2;
		}

	case 0xc8: {
		if (regs.read_flag("Z")) {
		const auto SP = regs.read("SP");
		const auto PC_new = static_cast<uint16_t>((memory.read(SP + 1) << 8) + memory.read(SP));
		regs.write("PC", PC_new - 1);
		regs.write("SP", SP + 2);
		return 5;
		}
		return 2;
		}

	case 0xd8: {
		if (regs.read_flag("C")) {
		const auto SP = regs.read("SP");
		const auto PC_new = static_cast<uint16_t>((memory.read(SP + 1) << 8) + memory.read(SP));
		regs.write("PC", PC_new - 1);
		regs.write("SP", SP + 2);
		return 5;
		}
		return 2;
		}

	case 0xd9: {
		regs.set_IME(true);
		
		const auto SP = regs.read("SP");
		const auto PC_new = static_cast<uint16_t>((memory.read(SP + 1) << 8) + memory.read(SP));
		regs.write("PC", PC_new - 1);
		regs.write("SP", SP + 2);
		
		return 4;
		}

	case 0xc9: {
		const auto SP = regs.read("SP");
		const auto PC_new = static_cast<uint16_t>((memory.read(SP + 1) << 8) + memory.read(SP));
		regs.write("PC", PC_new - 1);
		regs.write("SP", SP + 2);
		return 4;
		}

	case 0xc2: {
		if (!regs.read_flag("Z")) {
		const auto PC_new = static_cast<uint16_t>((memory.read(PC + 2) << 8) + memory.read(PC + 1));
		regs.write("PC", PC_new - 3);
		return 4;
		}
		return 3;
		}

	case 0xca: {
		if (regs.read_flag("Z")) {
		const auto PC_new = static_cast<uint16_t>((memory.read(PC + 2) << 8) + memory.read(PC + 1));
		regs.write("PC", PC_new - 3);
		return 4;
		}
		return 3;
		}

	case 0xd2: {
		if (!regs.read_flag("C")) {
		const auto PC_new = static_cast<uint16_t>((memory.read(PC + 2) << 8) + memory.read(PC + 1));
		regs.write("PC", PC_new - 3);
		return 4;
		}
		return 3;
		}

	case 0xda: {
		if (regs.read_flag("C")) {
		const auto PC_new = static_cast<uint16_t>((memory.read(PC + 2) << 8) + memory.read(PC + 1));
		regs.write("PC", PC_new - 3);
		return 4;
		}
		return 3;
		}

	case 0xc3: {
		// TODO: Put into method
		const auto PC_new = static_cast<uint16_t>((memory.read(PC + 2) << 8) + memory.read(PC + 1));
		regs.write("PC", PC_new - 3);
		return 4;
		}

	case 0xe9: {
		// TODO: Put into method
		const auto PC_new = regs.read("HL");
		regs.write("PC", PC_new - 1);
		return 1;
		}

	case 0xc4: {
		if (!regs.read_flag("Z")) {
		// TODO: Put into method
		const auto return_address = PC + 3;
		const auto return_address_high = static_cast<uint8_t>((return_address & 0xff00) >> 8);
		const auto return_address_low = static_cast<uint8_t>(return_address & 0x00ff);
		
		const auto SP = regs.read("SP");
		memory.write(SP - 1, return_address_high);
		memory.write(SP - 2, return_address_low);
		regs.write("SP", SP - 2);
		
		const auto call_address = static_cast<uint16_t>((memory.read(PC + 2) << 8) + memory.read(PC + 1));
		
		regs.write("PC", call_address - 3);
		return 6;
		}
		return 3;
		}

	case 0xcc: {
		if (regs.read_flag("Z")) {
		// TODO: Put into method
		const auto return_address = PC + 3;
		const auto return_address_high = static_cast<uint8_t>((return_address & 0xff00) >> 8);
		const auto return_address_low = static_cast<uint8_t>(return_address & 0x00ff);
		
		const auto SP = regs.read("SP");
		memory.write(SP - 1, return_address_high);
		memory.write(SP - 2, return_address_low);
		regs.write("SP", SP - 2);
		
		const auto call_address = static_cast<uint16_t>((memory.read(PC + 2) << 8) + memory.read(PC + 1));
		
		regs.write("PC", call_address - 3);
		return 6;
		}
		return 3;
		}

	case 0xd4: {
		if (!regs.read_flag("C")) {
		// TODO: Put into method
		const auto return_address = PC + 3;
		const auto return_address_high = static_cast<uint8_t>((return_address & 0xff00) >> 8);
		const auto return_address_low = static_cast<uint8_t>(return_address & 0x00ff);
		
		const auto SP = regs.read("SP");
		memory.write(SP - 1, return_address_high);
		memory.write(SP - 2, return_address_low);
		regs.write("SP", SP - 2);
		
		const auto call_address = static_cast<uint16_t>((memory.read(PC + 2) << 8) + memory.read(PC + 1));
		
		regs.write("PC", call_address - 3);
		return 6;
		}
		return 3;
		}

	case 0xdc: {
		if (regs.read_flag("C")) {
		// TODO: Put into method
		const auto return_address = PC + 3;
		const auto return_address_high = static_cast<uint8_t>((return_address & 0xff00) >> 8);
		const auto return_address_low = static_cast<uint8_t>(return_address & 0x00ff);
		
		const auto SP = regs.read("SP");
		memory.write(SP - 1, return_address_high);
		memory.write(SP - 2, return_address_low);
		regs.write("SP", SP - 2);
		
		const auto call_address = static_cast<uint16_t>((memory.read(PC + 2) << 8) + memory.read(PC + 1));
		
		regs.write("PC", call_address - 3);
		return 6;
		}
		return 3;
		}

	case 0xcd: {
		// TODO: Put into method
		const auto return_address = PC + 3;
		const auto return_address_high = static_cast<uint8_t>((return_address & 0xff00) >> 8);
		const auto return_address_low = static_cast<uint8_t>(return_address & 0x00ff);
		
		const auto SP = regs.read("SP");
		memory.write(SP - 1, return_address_high);
		memory.write(SP - 2, return_address_low);
		regs.write("SP", SP - 2);
		
		const auto call_address = static_cast<uint16_t>((memory.read(PC + 2) << 8) + memory.read(PC + 1));
		
		regs.write("PC", call_address - 3);
		return 6;
		}

	case 0xc7: {
		instruction_rst(0x00, regs, memory, PC);
		return 4;
		}

	case 0xd7: {
		instruction_rst(0x10, regs, memory, PC);
		return 4;
		}

	case 0xe7: {
		instruction_rst(0x20, regs, memory, PC);
		return 4;
		}

	case 0xf7: {
		instruction_rst(0x30, regs, memory, PC);
		return 4;
		}

	case 0xcf: {
		instruction_rst(0x08, regs, memory, PC);
		return 4;
		}

	case 0xdf: {
		instruction_rst(0x18, regs, memory, PC);
		return 4;
		}

	case 0xef: {
		instruction_rst(0x28, regs, memory, PC);
		return 4;
		}

	case 0xff: {
		instruction_rst(0x38, regs, memory, PC);
		return 4;
		}

	case 0x20: {
		if (!regs.read_flag("Z")) {
		const auto value = static_cast<int8_t>(memory.read(PC + 1));
		const auto PC_new = static_cast<uint16_t>(PC + value);
		regs.write("PC", PC_new - 2 + 2);
		return 3;
		}
		return 2;
		}

	case 0x30: {
		if (!regs.read_flag("C")) {
		const auto value = static_cast<int8_t>(memory.read(PC + 1));
		const auto PC_new = static_cast<uint16_t>(PC + value);
		regs.write("PC", PC_new - 2 + 2);
		return 3;
		}
		return 2;
		}


	case 0xcb00: {
		instruction_rlc("B", regs);
		return 2;
		}

	case 0xcb01: {
		instruction_rlc("C", regs);
		return 2;
		}

	case 0xcb02: {
		instruction_rlc("D", regs);
		return 2;
		}

	case 0xcb03: {
		instruction_rlc("E", regs);
		return 2;
		}

	case 0xcb04: {
		instruction_rlc("H", regs);
		return 2;
		}

	case 0xcb05: {
		instruction_rlc("L", regs);
		return 2;
		}

	case 0xcb06: {
		const auto HL = regs.read("HL");
		const auto value = memory.read(HL);
		const auto [new_value, carry] = rlc(value);


		memory.write(HL, new_value);
		set_flags_for_rotate(regs, new_value, carry);
		return 4;
		}

	case 0xcb07: {
		instruction_rlc("A", regs);
		return 2;
		}

	case 0xcb08: {
		instruction_rrc("B", regs);
		return 2;
		}

	case 0xcb09: {
		instruction_rrc("C", regs);
		return 2;
		}

	case 0xcb0a: {
		instruction_rrc("D", regs);
		return 2;
		}

	case 0xcb0b: {
		instruction_rrc("E", regs);
		return 2;
		}

	case 0xcb0c: {
		instruction_rrc("H", regs);
		return 2;
		}

	case 0xcb0d: {
		instruction_rrc("L", regs);
		return 2;
		}

	case 0xcb0e: {
		const auto HL = regs.read("HL");
		const auto value = memory.read(HL);
		const auto [new_value, carry] = rrc(value);

		memory.write(HL, new_value);
		set_flags_for_rotate(regs, new_value, carry);
		return 4;
		}

	case 0xcb0f: {
		instruction_rrc("A", regs);
		return 2;
		}

	case 0xcb10: {
		instruction_rl("B", regs);
		return 2;
		}

	case 0xcb11: {
		instruction_rl("C", regs);
		return 2;
		}

	case 0xcb12: {
		instruction_rl("D", regs);
		return 2;
		}

	case 0xcb13: {
		instruction_rl("E", regs);
		return 2;
		}

	case 0xcb14: {
		instruction_rl("H", regs);
		return 2;
		}

	case 0xcb15: {
		instruction_rl("L", regs);
		return 2;
		}

	case 0xcb16: {
		const auto HL = regs.read("HL");
		const auto value = memory.read(HL);
		const auto old_carry = regs.read_flag("C");
		const auto [new_value, carry] = rl(value, old_carry);

		memory.write(HL, new_value);
		set_flags_for_rotate(regs, new_value, carry);
		return 4;
		}

	case 0xcb17: {
		instruction_rl("A", regs);
		return 2;
		}

	case 0xcb18: {
		instruction_rr("B", regs);
		return 2;
		}

	case 0xcb19: {
		instruction_rr("C", regs);
		return 2;
		}

	case 0xcb1a: {
		instruction_rr("D", regs);
		return 2;
		}

	case 0xcb1b: {
		instruction_rr("E", regs);
		return 2;
		}

	case 0xcb1c: {
		instruction_rr("H", regs);
		return 2;
		}

	case 0xcb1d: {
		instruction_rr("L", regs);
		return 2;
		}

	case 0xcb1e: {
		const auto HL = regs.read("HL");
		const auto value = memory.read(HL);
		const auto old_carry = regs.read_flag("C");
		const auto [new_value, carry] = rr(value, old_carry);

		memory.write(HL, new_value);
		set_flags_for_rotate(regs, new_value, carry);
		return 4;
		}

	case 0xcb1f: {
		instruction_rr("A", regs);
		return 2;
		}

	case 0xcb20: {
		instruction_sla("B", regs);
		return 2;
		}

	case 0xcb21: {
		instruction_sla("C", regs);
		return 2;
		}

	case 0xcb22: {
		instruction_sla("D", regs);
		return 2;
		}

	case 0xcb23: {
		instruction_sla("E", regs);
		return 2;
		}

	case 0xcb24: {
		instruction_sla("H", regs);
		return 2;
		}

	case 0xcb25: {
		instruction_sla("L", regs);
		return 2;
		}

	case 0xcb26: {
		const auto HL = regs.read("HL");
		const auto value = memory.read(HL);
		const auto [new_value, carry] = sla(value);

		memory.write(HL, new_value);
		set_flags_for_shift(regs, new_value, carry);
		return 4;
		}

	case 0xcb27: {
		instruction_sla("A", regs);
		return 2;
		}

	case 0xcb28: {
		instruction_sra("B", regs);
		return 2;
		}

	case 0xcb29: {
		instruction_sra("C", regs);
		return 2;
		}

	case 0xcb2a: {
		instruction_sra("D", regs);
		return 2;
		}

	case 0xcb2b: {
		instruction_sra("E", regs);
		return 2;
		}

	case 0xcb2c: {
		instruction_sra("H", regs);
		return 2;
		}

	case 0xcb2d: {
		instruction_sra("L", regs);
		return 2;
		}

	case 0xcb2e: {
		const auto HL = regs.read("HL");
		const auto value = memory.read(HL);
		const auto [new_value, carry] = sra(value);

		memory.write(HL, new_value);
		set_flags_for_shift(regs, new_value, carry);
		return 4;
		}

	case 0xcb2f: {
		instruction_sra("A", regs);
		return 2;
		}

	case 0xcb30: {
		instruction_swap("B", regs);
		return 2;
		}

	case 0xcb31: {
		instruction_swap("C", regs);
		return 2;
		}

	case 0xcb32: {
		instruction_swap("D", regs);
		return 2;
		}

	case 0xcb33: {
		instruction_swap("E", regs);
		return 2;
		}

	case 0xcb34: {
		instruction_swap("H", regs);
		return 2;
		}

	case 0xcb35: {
		instruction_swap("L", regs);
		return 2;
		}

	case 0xcb36: {
		const auto HL = regs.read("HL");
		const auto value = memory.read(HL);
		const auto new_value = swap(value);

		memory.write(HL, new_value);
		set_flags_for_swap(regs, new_value);
		return 4;
		}

	case 0xcb37: {
		instruction_swap("A", regs);
		return 2;
		}

	case 0xcb38: {
		instruction_srl("B", regs);
		return 2;
		}

	case 0xcb39: {
		instruction_srl("C", regs);
		return 2;
		}

	case 0xcb3a: {
		instruction_srl("D", regs);
		return 2;
		}

	case 0xcb3b: {
		instruction_srl("E", regs);
		return 2;
		}

	case 0xcb3c: {
		instruction_srl("H", regs);
		return 2;
		}

	case 0xcb3d: {
		instruction_srl("L", regs);
		return 2;
		}

	case 0xcb3e: {
		const auto HL = regs.read("HL");
		const auto value = memory.read(HL);
		const auto [new_value, carry] = srl(value);

		memory.write(HL, new_value);
		set_flags_for_shift(regs, new_value, carry);
		return 4;
		}

	case 0xcb3f: {
		instruction_srl("A", regs);
		return 2;
		}

	case 0xcb40: {
		instruction_bit("B", 0, regs);
		return 2;
		}

	case 0xcb41: {
		instruction_bit("C", 0, regs);
		return 2;
		}

	case 0xcb42: {
		instruction_bit("D", 0, regs);
		return 2;
		}

	case 0xcb43: {
		instruction_bit("E", 0, regs);
		return 2;
		}

	case 0xcb44: {
		instruction_bit("H", 0, regs);
		return 2;
		}

	case 0xcb45: {
		instruction_bit("L", 0, regs);
		return 2;
		}

	case 0xcb46: {
		const auto HL = regs.read("HL");
		const auto value = memory.read(HL);
		bit(value, 0, regs);
		return 3;
		}

	case 0xcb47: {
		instruction_bit("A", 0, regs);
		return 2;
		}

	case 0xcb48: {
		instruction_bit("B", 1, regs);
		return 2;
		}

	case 0xcb49: {
		instruction_bit("C", 1, regs);
		return 2;
		}

	case 0xcb4a: {
		instruction_bit("D", 1, regs);
		return 2;
		}

	case 0xcb4b: {
		instruction_bit("E", 1, regs);
		return 2;
		}

	case 0xcb4c: {
		instruction_bit("H", 1, regs);
		return 2;
		}

	case 0xcb4d: {
		instruction_bit("L", 1, regs);
		return 2;
		}

	case 0xcb4e: {
		const auto HL = regs.read("HL");
		const auto value = memory.read(HL);
		bit(value, 1, regs);
		return 3;
		}

	case 0xcb4f: {
		instruction_bit("A", 1, regs);
		return 2;
		}

	case 0xcb50: {
		instruction_bit("B", 2, regs);
		return 2;
		}

	case 0xcb51: {
		instruction_bit("C", 2, regs);
		return 2;
		}

	case 0xcb52: {
		instruction_bit("D", 2, regs);
		return 2;
		}

	case 0xcb53: {
		instruction_bit("E", 2, regs);
		return 2;
		}

	case 0xcb54: {
		instruction_bit("H", 2, regs);
		return 2;
		}

	case 0xcb55: {
		instruction_bit("L", 2, regs);
		return 2;
		}

	case 0xcb56: {
		const auto HL = regs.read("HL");
		const auto value = memory.read(HL);
		bit(value, 2, regs);
		return 3;
		}

	case 0xcb57: {
		instruction_bit("A", 2, regs);
		return 2;
		}

	case 0xcb58: {
		instruction_bit("B", 3, regs);
		return 2;
		}

	case 0xcb59: {
		instruction_bit("C", 3, regs);
		return 2;
		}

	case 0xcb5a: {
		instruction_bit("D", 3, regs);
		return 2;
		}

	case 0xcb5b: {
		instruction_bit("E", 3, regs);
		return 2;
		}

	case 0xcb5c: {
		instruction_bit("H", 3, regs);
		return 2;
		}

	case 0xcb5d: {
		instruction_bit("L", 3, regs);
		return 2;
		}

	case 0xcb5e: {
		const auto HL = regs.read("HL");
		const auto value = memory.read(HL);
		bit(value, 3, regs);
		return 3;
		}

	case 0xcb5f: {
		instruction_bit("A", 3, regs);
		return 2;
		}

	case 0xcb60: {
		instruction_bit("B", 4, regs);
		return 2;
		}

	case 0xcb61: {
		instruction_bit("C", 4, regs);
		return 2;
		}

	case 0xcb62: {
		instruction_bit("D", 4, regs);
		return 2;
		}

	case 0xcb63: {
		instruction_bit("E", 4, regs);
		return 2;
		}

	case 0xcb64: {
		instruction_bit("H", 4, regs);
		return 2;
		}

	case 0xcb65: {
		instruction_bit("L", 4, regs);
		return 2;
		}

	case 0xcb66: {
		const auto HL = regs.read("HL");
		const auto value = memory.read(HL);
		bit(value, 4, regs);
		return 3;
		}

	case 0xcb67: {
		instruction_bit("A", 4, regs);
		return 2;
		}

	case 0xcb68: {
		instruction_bit("B", 5, regs);
		return 2;
		}

	case 0xcb69: {
		instruction_bit("C", 5, regs);
		return 2;
		}

	case 0xcb6a: {
		instruction_bit("D", 5, regs);
		return 2;
		}

	case 0xcb6b: {
		instruction_bit("E", 5, regs);
		return 2;
		}

	case 0xcb6c: {
		instruction_bit("H", 5, regs);
		return 2;
		}

	case 0xcb6d: {
		instruction_bit("L", 5, regs);
		return 2;
		}

	case 0xcb6e: {
		const auto HL = regs.read("HL");
		const auto value = memory.read(HL);
		bit(value, 5, regs);
		return 3;
		}

	case 0xcb6f: {
		instruction_bit("A", 5, regs);
		return 2;
		}

	case 0xcb70: {
		instruction_bit("B", 6, regs);
		return 2;
		}

	case 0xcb71: {
		instruction_bit("C", 6, regs);
		return 2;
		}

	case 0xcb72: {
		instruction_bit("D", 6, regs);
		return 2;
		}

	case 0xcb73: {
		instruction_bit("E", 6, regs);
		return 2;
		}

	case 0xcb74: {
		instruction_bit("H", 6, regs);
		return 2;
		}

	case 0xcb75: {
		instruction_bit("L", 6, regs);
		return 2;
		}

	case 0xcb76: {
		const auto HL = regs.read("HL");
		const auto value = memory.read(HL);
		bit(value, 6, regs);
		return 3;
		}

	case 0xcb77: {
		instruction_bit("A", 6, regs);
		return 2;
		}

	case 0xcb78: {
		instruction_bit("B", 7, regs);
		return 2;
		}

	case 0xcb79: {
		instruction_bit("C", 7, regs);
		return 2;
		}

	case 0xcb7a: {
		instruction_bit("D", 7, regs);
		return 2;
		}

	case 0xcb7b: {
		instruction_bit("E", 7, regs);
		return 2;
		}

	case 0xcb7c: {
		instruction_bit("H", 7, regs);
		return 2;
		}

	case 0xcb7d: {
		instruction_bit("L", 7, regs);
		return 2;
		}

	case 0xcb7e: {
		const auto HL = regs.read("HL");
		const auto value = memory.read(HL);
		bit(value, 7, regs);
		return 3;
		}

	case 0xcb7f: {
		instruction_bit("A", 7, regs);
		return 2;
		}

	case 0xcb80: {
		instruction_reset_bit("B", 0, regs);
		return 2;
		}

	case 0xcb81: {
		instruction_reset_bit("C", 0, regs);
		return 2;
		}

	case 0xcb82: {
		instruction_reset_bit("D", 0, regs);
		return 2;
		}

	case 0xcb83: {
		instruction_reset_bit("E", 0, regs);
		return 2;
		}

	case 0xcb84: {
		instruction_reset_bit("H", 0, regs);
		return 2;
		}

	case 0xcb85: {
		instruction_reset_bit("L", 0, regs);
		return 2;
		}

	case 0xcb86: {
		const auto HL = regs.read("HL");
		const auto old_value = memory.read(HL);

		auto new_value = reset_bit(old_value, 0);
		memory.write(HL, new_value);
		return 4;
		}

	case 0xcb87: {
		instruction_reset_bit("A", 0, regs);
		return 2;
		}

	case 0xcb88: {
		instruction_reset_bit("B", 1, regs);
		return 2;
		}

	case 0xcb89: {
		instruction_reset_bit("C", 1, regs);
		return 2;
		}

	case 0xcb8a: {
		instruction_reset_bit("D", 1, regs);
		return 2;
		}

	case 0xcb8b: {
		instruction_reset_bit("E", 1, regs);
		return 2;
		}

	case 0xcb8c: {
		instruction_reset_bit("H", 1, regs);
		return 2;
		}

	case 0xcb8d: {
		instruction_reset_bit("L", 1, regs);
		return 2;
		}

	case 0xcb8e: {
		const auto HL = regs.read("HL");
		const auto old_value = memory.read(HL);

		auto new_value = reset_bit(old_value, 1);
		memory.write(HL, new_value);
		return 4;
		}

	case 0xcb8f: {
		instruction_reset_bit("A", 1, regs);
		return 2;
		}

	case 0xcb90: {
		instruction_reset_bit("B", 2, regs);
		return 2;
		}

	case 0xcb91: {
		instruction_reset_bit("C", 2, regs);
		return 2;
		}

	case 0xcb92: {
		instruction_reset_bit("D", 2, regs);
		return 2;
		}

	case 0xcb93: {
		instruction_reset_bit("E", 2, regs);
		return 2;
		}

	case 0xcb94: {
		instruction_reset_bit("H", 2, regs);
		return 2;
		}

	case 0xcb95: {
		instruction_reset_bit("L", 2, regs);
		return 2;
		}

	case 0xcb96: {
		const auto HL = regs.read("HL");
		const auto old_value = memory.read(HL);

		auto new_value = reset_bit(old_value, 2);
		memory.write(HL, new_value);
		return 4;
		}

	case 0xcb97: {
		instruction_reset_bit("A", 2, regs);
		return 2;
		}

	case 0xcb98: {
		instruction_reset_bit("B", 3, regs);
		return 2;
		}

	case 0xcb99: {
		instruction_reset_bit("C", 3, regs);
		return 2;
		}

	case 0xcb9a: {
		instruction_reset_bit("D", 3, regs);
		return 2;
		}

	case 0xcb9b: {
		instruction_reset_bit("E", 3, regs);
		return 2;
		}

	case 0xcb9c: {
		instruction_reset_bit("H", 3, regs);
		return 2;
		}

	case 0xcb9d: {
		instruction_reset_bit("L", 3, regs);
		return 2;
		}

	case 0xcb9e: {
		const auto HL = regs.read("HL");
		const auto old_value = memory.read(HL);

		auto new_value = reset_bit(old_value, 3);
		memory.write(HL, new_value);
		return 4;
		}

	case 0xcb9f: {
		instruction_reset_bit("A", 3, regs);
		return 2;
		}

	case 0xcba0: {
		instruction_reset_bit("B", 4, regs);
		return 2;
		}

	case 0xcba1: {
		instruction_reset_bit("C", 4, regs);
		return 2;
		}

	case 0xcba2: {
		instruction_reset_bit("D", 4, regs);
		return 2;
		}

	case 0xcba3: {
		instruction_reset_bit("E", 4, regs);
		return 2;
		}

	case 0xcba4: {
		instruction_reset_bit("H", 4, regs);
		return 2;
		}

	case 0xcba5: {
		instruction_reset_bit("L", 4, regs);
		return 2;
		}

	case 0xcba6: {
		const auto HL = regs.read("HL");
		const auto old_value = memory.read(HL);

		auto new_value = reset_bit(old_value, 4);
		memory.write(HL, new_value);
		return 4;
		}

	case 0xcba7: {
		instruction_reset_bit("A", 4, regs);
		return 2;
		}

	case 0xcba8: {
		instruction_reset_bit("B", 5, regs);
		return 2;
		}

	case 0xcba9: {
		instruction_reset_bit("C", 5, regs);
		return 2;
		}

	case 0xcbaa: {
		instruction_reset_bit("D", 5, regs);
		return 2;
		}

	case 0xcbab: {
		instruction_reset_bit("E", 5, regs);
		return 2;
		}

	case 0xcbac: {
		instruction_reset_bit("H", 5, regs);
		return 2;
		}

	case 0xcbad: {
		instruction_reset_bit("L", 5, regs);
		return 2;
		}

	case 0xcbae: {
		const auto HL = regs.read("HL");
		const auto old_value = memory.read(HL);

		auto new_value = reset_bit(old_value, 5);
		memory.write(HL, new_value);
		return 4;
		}

	case 0xcbaf: {
		instruction_reset_bit("A", 5, regs);
		return 2;
		}

	case 0xcbb0: {
		instruction_reset_bit("B", 6, regs);
		return 2;
		}

	case 0xcbb1: {
		instruction_reset_bit("C", 6, regs);
		return 2;
		}

	case 0xcbb2: {
		instruction_reset_bit("D", 6, regs);
		return 2;
		}

	case 0xcbb3: {
		instruction_reset_bit("E", 6, regs);
		return 2;
		}

	case 0xcbb4: {
		instruction_reset_bit("H", 6, regs);
		return 2;
		}

	case 0xcbb5: {
		instruction_reset_bit("L", 6, regs);
		return 2;
		}

	case 0xcbb6: {
		const auto HL = regs.read("HL");
		const auto old_value = memory.read(HL);

		auto new_value = reset_bit(old_value, 6);
		memory.write(HL, new_value);
		return 4;
		}

	case 0xcbb7: {
		instruction_reset_bit("A", 6, regs);
		return 2;
		}

	case 0xcbb8: {
		instruction_reset_bit("B", 7, regs);
		return 2;
		}

	case 0xcbb9: {
		instruction_reset_bit("C", 7, regs);
		return 2;
		}

	case 0xcbba: {
		instruction_reset_bit("D", 7, regs);
		return 2;
		}

	case 0xcbbb: {
		instruction_reset_bit("E", 7, regs);
		return 2;
		}

	case 0xcbbc: {
		instruction_reset_bit("H", 7, regs);
		return 2;
		}

	case 0xcbbd: {
		instruction_reset_bit("L", 7, regs);
		return 2;
		}

	case 0xcbbe: {
		const auto HL = regs.read("HL");
		const auto old_value = memory.read(HL);

		auto new_value = reset_bit(old_value, 7);
		memory.write(HL, new_value);
		return 4;
		}

	case 0xcbbf: {
		instruction_reset_bit("A", 7, regs);
		return 2;
		}

	case 0xcbc0: {
		instruction_set_bit("B", 0, regs);
		return 2;
		}

	case 0xcbc1: {
		instruction_set_bit("C", 0, regs);
		return 2;
		}

	case 0xcbc2: {
		instruction_set_bit("D", 0, regs);
		return 2;
		}

	case 0xcbc3: {
		instruction_set_bit("E", 0, regs);
		return 2;
		}

	case 0xcbc4: {
		instruction_set_bit("H", 0, regs);
		return 2;
		}

	case 0xcbc5: {
		instruction_set_bit("L", 0, regs);
		return 2;
		}

	case 0xcbc6: {
		return instruction_set_bit_hl(memory, regs, 0);
		}

	case 0xcbc7: {
		instruction_set_bit("A", 0, regs);
		return 2;
		}

	case 0xcbc8: {
		instruction_set_bit("B", 1, regs);
		return 2;
		}

	case 0xcbc9: {
		instruction_set_bit("C", 1, regs);
		return 2;
		}

	case 0xcbca: {
		instruction_set_bit("D", 1, regs);
		return 2;
		}

	case 0xcbcb: {
		instruction_set_bit("E", 1, regs);
		return 2;
		}

	case 0xcbcc: {
		instruction_set_bit("H", 1, regs);
		return 2;
		}

	case 0xcbcd: {
		instruction_set_bit("L", 1, regs);
		return 2;
		}

	case 0xcbce: {
		return instruction_set_bit_hl(memory, regs, 1);
		}

	case 0xcbcf: {
		instruction_set_bit("A", 1, regs);
		return 2;
		}

	case 0xcbd0: {
		instruction_set_bit("B", 2, regs);
		return 2;
		}

	case 0xcbd1: {
		instruction_set_bit("C", 2, regs);
		return 2;
		}

	case 0xcbd2: {
		instruction_set_bit("D", 2, regs);
		return 2;
		}

	case 0xcbd3: {
		instruction_set_bit("E", 2, regs);
		return 2;
		}

	case 0xcbd4: {
		instruction_set_bit("H", 2, regs);
		return 2;
		}

	case 0xcbd5: {
		instruction_set_bit("L", 2, regs);
		return 2;
		}

	case 0xcbd6: {
		return instruction_set_bit_hl(memory, regs, 2);
		}

	case 0xcbd7: {
		instruction_set_bit("A", 2, regs);
		return 2;
		}

	case 0xcbd8: {
		instruction_set_bit("B", 3, regs);
		return 2;
		}

	case 0xcbd9: {
		instruction_set_bit("C", 3, regs);
		return 2;
		}

	case 0xcbda: {
		instruction_set_bit("D", 3, regs);
		return 2;
		}

	case 0xcbdb: {
		instruction_set_bit("E", 3, regs);
		return 2;
		}

	case 0xcbdc: {
		instruction_set_bit("H", 3, regs);
		return 2;
		}

	case 0xcbdd: {
		instruction_set_bit("L", 3, regs);
		return 2;
		}

	case 0xcbde: {
		return instruction_set_bit_hl(memory, regs, 3);
		}

	case 0xcbdf: {
		instruction_set_bit("A", 3, regs);
		return 2;
		}

	case 0xcbe0: {
		instruction_set_bit("B", 4, regs);
		return 2;
		}

	case 0xcbe1: {
		instruction_set_bit("C", 4, regs);
		return 2;
		}

	case 0xcbe2: {
		instruction_set_bit("D", 4, regs);
		return 2;
		}

	case 0xcbe3: {
		instruction_set_bit("E", 4, regs);
		return 2;
		}

	case 0xcbe4: {
		instruction_set_bit("H", 4, regs);
		return 2;
		}

	case 0xcbe5: {
		instruction_set_bit("L", 4, regs);
		return 2;
		}

	case 0xcbe6: {
		return instruction_set_bit_hl(memory, regs, 4);
		}

	case 0xcbe7: {
		instruction_set_bit("A", 4, regs);
		return 2;
		}

	case 0xcbe8: {
		instruction_set_bit("B", 5, regs);
		return 2;
		}

	case 0xcbe9: {
		instruction_set_bit("C", 5, regs);
		return 2;
		}

	case 0xcbea: {
		instruction_set_bit("D", 5, regs);
		return 2;
		}

	case 0xcbeb: {
		instruction_set_bit("E", 5, regs);
		return 2;
		}

	case 0xcbec: {
		instruction_set_bit("H", 5, regs);
		return 2;
		}

	case 0xcbed: {
		instruction_set_bit("L", 5, regs);
		return 2;
		}

	case 0xcbee: {
		return instruction_set_bit_hl(memory, regs, 5);
		}

	case 0xcbef: {
		instruction_set_bit("A", 5, regs);
		return 2;
		}

	case 0xcbf0: {
		instruction_set_bit("B", 6, regs);
		return 2;
		}

	case 0xcbf1: {
		instruction_set_bit("C", 6, regs);
		return 2;
		}

	case 0xcbf2: {
		instruction_set_bit("D", 6, regs);
		return 2;
		}

	case 0xcbf3: {
		instruction_set_bit("E", 6, regs);
		return 2;
		}

	case 0xcbf4: {
		instruction_set_bit("H", 6, regs);
		return 2;
		}

	case 0xcbf5: {
		instruction_set_bit("L", 6, regs);
		return 2;
		}

	case 0xcbf6: {
		return instruction_set_bit_hl(memory, regs, 6);
		}

	case 0xcbf7: {
		instruction_set_bit("A", 6, regs);
		return 2;
		}

	case 0xcbf8: {
		instruction_set_bit("B", 7, regs);
		return 2;
		}

	case 0xcbf9: {
		instruction_set_bit("C", 7, regs);
		return 2;
		}

	case 0xcbfa: {
		instruction_set_bit("D", 7, regs);
		return 2;
		}

	case 0xcbfb: {
		instruction_set_bit("E", 7, regs);
		return 2;
		}

	case 0xcbfc: {
		instruction_set_bit("H", 7, regs);
		return 2;
		}

	case 0xcbfd: {
		instruction_set_bit("L", 7, regs);
		return 2;
		}

	case 0xcbfe: {
		return instruction_set_bit_hl(memory, regs, 7);
		}

	case 0xcbff: {
		instruction_set_bit("A", 7, regs);
		return 2;
		}
	}

}
