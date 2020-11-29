#pragma once

#include "instruction_utils.h"

auto get_16bit_instructions() {
	return std::vector<Instruction>{
		// RLC
		{"RLC B", 0xcb00, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rlc("B", regs);
				return 2;
			}
		},
		{"RLC C", 0xcb01, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rlc("C", regs);
				return 2;
			}
		},
		{"RLC D", 0xcb02, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rlc("D", regs);
				return 2;
			}
		},
		{"RLC E", 0xcb03, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rlc("E", regs);
				return 2;
			}
		},
		{"RLC H", 0xcb04, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rlc("H", regs);
				return 2;
			}
		},
		{"RLC L", 0xcb05, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rlc("L", regs);
				return 2;
			}
		},
		{"RLC (HL)", 0xcb06, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto value = memory.read(address);
				const auto [new_value, carry] = rlc(value);

				memory.write(address, new_value);
				set_flags_for_rotate(regs, new_value, carry);
				return 4;
			}
		},
		{"RLC A", 0xcb07, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rlc("A", regs);
				return 2;
			}
		},

		// RRC
		{"RRC B", 0xcb08, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rrc("B", regs);
				return 2;
			}
		},
		{"RRC C", 0xcb09, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rrc("C", regs);
				return 2;
			}
		},
		{"RRC D", 0xcb0a, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rrc("D", regs);
				return 2;
			}
		},
		{"RRC E", 0xcb0b, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rrc("E", regs);
				return 2;
			}
		},
		{"RRC H", 0xcb0c, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rrc("H", regs);
				return 2;
			}
		},
		{"RRC L", 0xcb0d, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rrc("R", regs);
				return 2;
			}
		},
		{"RRC (HL)", 0xcb0e, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto value = memory.read(address);
				const auto [new_value, carry] = rrc(value);

				memory.write(address, new_value);
				set_flags_for_rotate(regs, new_value, carry);
				return 4;
			}
		},
		{"RRC A", 0xcb0f, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rrc("A", regs);
				return 2;
			}
		},

		// RL
		{"RL B", 0xcb10, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rl("B", regs);
				return 2;
			}
		},
		{"RL C", 0xcb11, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rl("C", regs);
				return 2;
			}
		},
		{"RL D", 0xcb12, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rl("D", regs);
				return 2;
			}
		},
		{"RL E", 0xcb13, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rl("E", regs);
				return 2;
			}
		},
		{"RL H", 0xcb14, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rl("H", regs);
				return 2;
			}
		},
		{"RL L", 0xcb15, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rl("R", regs);
				return 2;
			}
		},
		{"RL (HL)", 0xcb16, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto value = memory.read(address);
				const auto old_carry = regs.read_flag("C");
				const auto [new_value, carry] = rl(value, old_carry);

				memory.write(address, new_value);
				set_flags_for_rotate(regs, new_value, carry);
				return 4;
			}
		},
		{"RL A", 0xcb17, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rl("A", regs);
				return 2;
			}
		},

		// RR
		{"RR B", 0xcb18, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rr("B", regs);
				return 2;
			}
		},
		{"RR C", 0xcb19, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rr("C", regs);
				return 2;
			}
		},
		{"RR D", 0xcb1a, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rr("D", regs);
				return 2;
			}
		},
		{"RR E", 0xcb1b, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rr("E", regs);
				return 2;
			}
		},
		{"RR H", 0xcb1c, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rr("H", regs);
				return 2;
			}
		},
		{"RR L", 0xcb1d, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rr("L", regs);
				return 2;
			}
		},
		{"RR (HL)", 0xcb1e, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto value = memory.read(address);
				const auto old_carry = regs.read_flag("C");
				const auto [new_value, carry] = rr(value, old_carry);

				memory.write(address, new_value);
				set_flags_for_rotate(regs, new_value, carry);
				return 4;
			}
		},
		{"RR A", 0xcb1f, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_rr("A", regs);
				return 2;
			}
		},

		// SLA
		{"SLA B", 0xcb20, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sla("B", regs);
				return 2;
			}
		},
		{"SLA C", 0xcb21, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sla("C", regs);
				return 2;
			}
		},
		{"SLA D", 0xcb22, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sla("D", regs);
				return 2;
			}
		},
		{"SLA E", 0xcb23, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sla("E", regs);
				return 2;
			}
		},
		{"SLA H", 0xcb24, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sla("H", regs);
				return 2;
			}
		},
		{"SLA L", 0xcb25, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sla("L", regs);
				return 2;
			}
		},
		{"SLA (HL)", 0xcb26, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto value = memory.read(address);
				const auto [new_value, carry] = sla(value);

				memory.write(address, new_value);
				set_flags_for_shift(regs, new_value, carry);
				return 4;
			}
		},
		{"SLA A", 0xcb27, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sla("A", regs);
				return 2;
			}
		},

		// SRA
		{"SRA B", 0xcb28, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sra("B", regs);
				return 2;
			}
		},
		{"SRA C", 0xcb29, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sra("C", regs);
				return 2;
			}
		},
		{"SRA D", 0xcb2a, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sra("D", regs);
				return 2;
			}
		},
		{"SRA E", 0xcb2b, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sra("E", regs);
				return 2;
			}
		},
		{"SRA H", 0xcb2c, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sra("H", regs);
				return 2;
			}
		},
		{"SRA L", 0xcb2d, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sra("L", regs);
				return 2;
			}
		},
		{"SRA (HL)", 0xcb2e, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto value = memory.read(address);
				const auto [new_value, carry] = sra(value);

				memory.write(address, new_value);
				set_flags_for_shift(regs, new_value, carry);
				return 4;
			}
		},
		{"SRA A", 0xcb2f, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_sra("A", regs);
				return 2;
			}
		},

		// SWAP
		{"SWAP B", 0xcb30, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_swap("B", regs);
				return 2;
			}
		},
		{"SWAP C", 0xcb31, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_swap("C", regs);
				return 2;
			}
		},
		{"SWAP D", 0xcb32, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_swap("D", regs);
				return 2;
			}
		},
		{"SWAP E", 0xcb33, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_swap("E", regs);
				return 2;
			}
		},
		{"SWAP H", 0xcb34, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_swap("H", regs);
				return 2;
			}
		},
		{"SWAP L", 0xcb35, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_swap("L", regs);
				return 2;
			}
		},
		{"SWAP (HL)", 0xcb36, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto value = memory.read(address);
				const auto new_value = swap(value);

				memory.write(address, new_value);
				set_flags_for_swap(regs, new_value);
				return 4;
			}
		},
		{"SWAP A", 0xcb37, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_swap("A", regs);
				return 2;
			}
		},

		// SRL
		{"SRL B", 0xcb38, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_srl("B", regs);
				return 2;
			}
		},
		{"SRL C", 0xcb39, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_srl("C", regs);
				return 2;
			}
		},
		{"SRL D", 0xcb3a, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_srl("D", regs);
				return 2;
			}
		},
		{"SRL E", 0xcb3b, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_srl("E", regs);
				return 2;
			}
		},
		{"SRL H", 0xcb3c, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_srl("H", regs);
				return 2;
			}
		},
		{"SRL L", 0xcb3d, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_srl("L", regs);
				return 2;
			}
		},
		{"SRL (HL)", 0xcb3e, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto value = memory.read(address);
				const auto [new_value, carry] = srl(value);

				memory.write(address, new_value);
				set_flags_for_shift(regs, new_value, carry);
				return 4;
			}
		},
		{"SRL A", 0xcb3f, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_srl("A", regs);
				return 2;
			}
		},

		// BIT
		{"BIT 0, B", 0xcb40, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("B", 0, regs);
				return 2;
			}
		},
		{"BIT 0, C", 0xcb41, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("C", 0, regs);
				return 2;
			}
		},
		{"BIT 0, D", 0xcb42, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("D", 0, regs);
				return 2;
			}
		},
		{"BIT 0, E", 0xcb43, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("E", 0, regs);
				return 2;
			}
		},
		{"BIT 0, H", 0xcb44, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("H", 0, regs);
				return 2;
			}
		},
		{"BIT 0, L", 0xcb45, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("L", 0, regs);
				return 2;
			}
		},
		{"BIT 0, (HL)", 0xcb46, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto value = memory.read(address);
				bit(value, 0, regs);
				return 4;
			}
		},
		{"BIT 0, A", 0xcb47, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("A", 0, regs);
				return 2;
			}
		},
		{"BIT 1, B", 0xcb48, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("B", 1, regs);
				return 2;
			}
		},
		{"BIT 1, C", 0xcb49, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("C", 1, regs);
				return 2;
			}
		},
		{"BIT 1, D", 0xcb4a, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("D", 1, regs);
				return 2;
			}
		},
		{"BIT 1, E", 0xcb4b, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("E", 1, regs);
				return 2;
			}
		},
		{"BIT 1, H", 0xcb4c, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("H", 1, regs);
				return 2;
			}
		},
		{"BIT 1, L", 0xcb4d, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("L", 1, regs);
				return 2;
			}
		},
		{"BIT 1, (HL)", 0xcb4e, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto value = memory.read(address);
				bit(value, 1, regs);
				return 4;
			}
		},
		{"BIT 1, A", 0xcb4f, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("A", 1, regs);
				return 2;
			}
		},

		{"BIT 2, B", 0xcb50, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("B", 2, regs);
				return 2;
			}
		},
		{"BIT 2, C", 0xcb51, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("C", 2, regs);
				return 2;
			}
		},
		{"BIT 2, D", 0xcb52, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("D", 2, regs);
				return 2;
			}
		},
		{"BIT 2, E", 0xcb53, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("E", 2, regs);
				return 2;
			}
		},
		{"BIT 2, H", 0xcb54, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("H", 2, regs);
				return 2;
			}
		},
		{"BIT 2, L", 0xcb55, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("L", 2, regs);
				return 2;
			}
		},
		{"BIT 2, (HL)", 0xcb56, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto value = memory.read(address);
				bit(value, 2, regs);
				return 4;
			}
		},
		{"BIT 2, A", 0xcb57, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("A", 2, regs);
				return 2;
			}
		},
		{"BIT 3, B", 0xcb58, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("B", 3, regs);
				return 2;
			}
		},
		{"BIT 3, C", 0xcb59, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("C", 3, regs);
				return 2;
			}
		},
		{"BIT 3, D", 0xcb5a, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("D", 3, regs);
				return 2;
			}
		},
		{"BIT 3, E", 0xcb5b, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("E", 3, regs);
				return 2;
			}
		},
		{"BIT 3, H", 0xcb5c, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("H", 3, regs);
				return 2;
			}
		},
		{"BIT 3, L", 0xcb5d, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("L", 3, regs);
				return 2;
			}
		},
		{"BIT 3, (HL)", 0xcb5e, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto value = memory.read(address);
				bit(value, 3, regs);
				return 4;
			}
		},
		{"BIT 3, A", 0xcb5f, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("A", 3, regs);
				return 2;
			}
		},

		{"BIT 4, B", 0xcb60, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("B", 4, regs);
				return 2;
			}
		},
		{"BIT 4, C", 0xcb61, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("C", 4, regs);
				return 2;
			}
		},
		{"BIT 4, D", 0xcb62, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("D", 4, regs);
				return 2;
			}
		},
		{"BIT 4, E", 0xcb63, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("E", 4, regs);
				return 2;
			}
		},
		{"BIT 4, H", 0xcb64, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("H", 4, regs);
				return 2;
			}
		},
		{"BIT 4, L", 0xcb65, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("L", 4, regs);
				return 2;
			}
		},
		{"BIT 4, (HL)", 0xcb66, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto value = memory.read(address);
				bit(value, 4, regs);
				return 4;
			}
		},
		{"BIT 4, A", 0xcb67, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("A", 4, regs);
				return 2;
			}
		},
		{"BIT 5, B", 0xcb68, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("B", 5, regs);
				return 2;
			}
		},
		{"BIT 5, C", 0xcb69, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("C", 5, regs);
				return 2;
			}
		},
		{"BIT 5, D", 0xcb6a, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("D", 5, regs);
				return 2;
			}
		},
		{"BIT 5, E", 0xcb6b, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("E", 5, regs);
				return 2;
			}
		},
		{"BIT 5, H", 0xcb6c, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("H", 5, regs);
				return 2;
			}
		},
		{"BIT 5, L", 0xcb6d, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("L", 5, regs);
				return 2;
			}
		},
		{"BIT 5, (HL)", 0xcb6e, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto value = memory.read(address);
				bit(value, 5, regs);
				return 4;
			}
		},
		{"BIT 5, A", 0xcb6f, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("A", 5, regs);
				return 2;
			}
		},

		{"BIT 6, B", 0xcb70, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("B", 6, regs);
				return 2;
			}
		},
		{"BIT 6, C", 0xcb71, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("C", 6, regs);
				return 2;
			}
		},
		{"BIT 6, D", 0xcb72, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("D", 6, regs);
				return 2;
			}
		},
		{"BIT 6, E", 0xcb73, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("E", 6, regs);
				return 2;
			}
		},
		{"BIT 6, H", 0xcb74, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("H", 6, regs);
				return 2;
			}
		},
		{"BIT 6, L", 0xcb75, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("L", 6, regs);
				return 2;
			}
		},
		{"BIT 6, (HL)", 0xcb76, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto value = memory.read(address);
				bit(value, 6, regs);
				return 4;
			}
		},
		{"BIT 6, A", 0xcb77, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("A", 6, regs);
				return 2;
			}
		},
		{"BIT 7, B", 0xcb78, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("B", 7, regs);
				return 2;
			}
		},
		{"BIT 7, C", 0xcb79, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("C", 7, regs);
				return 2;
			}
		},
		{"BIT 7, D", 0xcb7a, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("D", 7, regs);
				return 2;
			}
		},
		{"BIT 7, E", 0xcb7b, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("E", 7, regs);
				return 2;
			}
		},
		{"BIT 7, H", 0xcb7c, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("H", 7, regs);
				return 2;
			}
		},
		{"BIT 7, L", 0xcb7d, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("L", 7, regs);
				return 2;
			}
		},
		{"BIT 7, (HL)", 0xcb7e, 2,
			[](auto& regs, auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto value = memory.read(address);
				bit(value, 7, regs);
				return 4;
			}
		},
		{"BIT 7, A", 0xcb7f, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_bit("A", 7, regs);
				return 2;
			}
		},

		// RES
		{"RES 0, B", 0xcb80, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("B", 0, regs);
				return 2;
			}
		},
		{"RES 0, C", 0xcb81, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("C", 0, regs);
				return 2;
			}
		},
		{"RES 0, D", 0xcb82, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("D", 0, regs);
				return 2;
			}
		},
		{"RES 0, E", 0xcb83, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("E", 0, regs);
				return 2;
			}
		},
		{"RES 0, H", 0xcb84, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("H", 0, regs);
				return 2;
			}
		},
		{"RES 0, L", 0xcb85, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("L", 0, regs);
				return 2;
			}
		},
		{"RES 0, (HL)", 0xcb86, 2,
			[]([[maybe_unused]] auto& regs, auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto old_value = memory.read(address);

				auto new_value = reset_bit(old_value, 0);
				memory.write(address, new_value);
				return 4;
			}
		},
		{"RES 0, A", 0xcb87, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("A", 0, regs);
				return 2;
			}
		},
		{"RES 1, B", 0xcb88, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("B", 1, regs);
				return 2;
			}
		},
		{"RES 1, C", 0xcb89, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("C", 1, regs);
				return 2;
			}
		},
		{"RES 1, D", 0xcb8a, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("D", 1, regs);
				return 2;
			}
		},
		{"RES 1, E", 0xcb8b, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("E", 1, regs);
				return 2;
			}
		},
		{"RES 1, H", 0xcb8c, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("H", 1, regs);
				return 2;
			}
		},
		{"RES 1, L", 0xcb8d, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("L", 1, regs);
				return 2;
			}
		},
		{"RES 1, (HL)", 0xcb8e, 2,
			[]([[maybe_unused]] auto& regs, auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto old_value = memory.read(address);

				auto new_value = reset_bit(old_value, 1);
				memory.write(address, new_value);
				return 4;
			}
		},
		{"RES 1, A", 0xcb8f, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("A", 1, regs);
				return 2;
			}
		},

		{"RES 2, B", 0xcb90, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("B", 2, regs);
				return 2;
			}
		},
		{"RES 2, C", 0xcb91, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("C", 2, regs);
				return 2;
			}
		},
		{"RES 2, D", 0xcb92, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("D", 2, regs);
				return 2;
			}
		},
		{"RES 2, E", 0xcb93, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("E", 2, regs);
				return 2;
			}
		},
		{"RES 2, H", 0xcb94, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("H", 2, regs);
				return 2;
			}
		},
		{"RES 2, L", 0xcb95, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("L", 2, regs);
				return 2;
			}
		},
		{"RES 2, (HL)", 0xcb96, 2,
			[]([[maybe_unused]] auto& regs, auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto old_value = memory.read(address);

				auto new_value = reset_bit(old_value, 2);
				memory.write(address, new_value);
				return 4;
			}
		},
		{"RES 2, A", 0xcb97, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("A", 2, regs);
				return 2;
			}
		},
		{"RES 3, B", 0xcb98, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("B", 3, regs);
				return 2;
			}
		},
		{"RES 3, C", 0xcb99, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("C", 3, regs);
				return 2;
			}
		},
		{"RES 3, D", 0xcb9a, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("D", 3, regs);
				return 2;
			}
		},
		{"RES 3, E", 0xcb9b, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("E", 3, regs);
				return 2;
			}
		},
		{"RES 3, H", 0xcb9c, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("H", 3, regs);
				return 2;
			}
		},
		{"RES 3, L", 0xcb9d, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("L", 3, regs);
				return 2;
			}
		},
		{"RES 3, (HL)", 0xcb9e, 2,
			[]([[maybe_unused]] auto& regs,  auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto old_value = memory.read(address);

				auto new_value = reset_bit(old_value, 3);
				memory.write(address, new_value);
				return 4;
			}
		},
		{"RES 3, A", 0xcb9f, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("A", 3, regs);
				return 2;
			}
		},

		{"RES 4, B", 0xcba0, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("B", 4, regs);
				return 2;
			}
		},
		{"RES 4, C", 0xcba1, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("C", 4, regs);
				return 2;
			}
		},
		{"RES 4, D", 0xcba2, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("D", 4, regs);
				return 2;
			}
		},
		{"RES 4, E", 0xcba3, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("E", 4, regs);
				return 2;
			}
		},
		{"RES 4, H", 0xcba4, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("H", 4, regs);
				return 2;
			}
		},
		{"RES 4, L", 0xcba5, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("L", 4, regs);
				return 2;
			}
		},
		{"RES 4, (HL)", 0xcba6, 2,
			[]([[maybe_unused]] auto& regs, auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto old_value = memory.read(address);

				auto new_value = reset_bit(old_value, 4);
				memory.write(address, new_value);
				return 4;
			}
		},
		{"RES 4, A", 0xcba7, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("A", 4, regs);
				return 2;
			}
		},
		{"RES 5, B", 0xcba8, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("B", 5, regs);
				return 2;
			}
		},
		{"RES 5, C", 0xcba9, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("C", 5, regs);
				return 2;
			}
		},
		{"RES 5, D", 0xcbaa, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("D", 5, regs);
				return 2;
			}
		},
		{"RES 5, E", 0xcbab, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("E", 5, regs);
				return 2;
			}
		},
		{"RES 5, H", 0xcbac, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("H", 5, regs);
				return 2;
			}
		},
		{"RES 5, L", 0xcbad, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("L", 5, regs);
				return 2;
			}
		},
		{"RES 5, (HL)", 0xcbae, 2,
			[]([[maybe_unused]] auto& regs,  auto& memory, const auto& PC) {
				const auto address = memory.read(PC + 1);
				const auto old_value = memory.read(address);

				auto new_value = reset_bit(old_value, 5);
				memory.write(address, new_value);
				return 4;
			}
		},
		{"RES 5, A", 0xcbaf, 2,
			[](auto& regs, [[maybe_unused]] auto& memory, [[maybe_unused]] const auto& PC) {
				instruction_reset_bit("A", 5, regs);
				return 2;
			}
		},

	};
}
