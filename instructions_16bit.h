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

	};
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
				const auto [new_value, carry] = swap(value);

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

	};
}
