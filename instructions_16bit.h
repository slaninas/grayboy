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

	};
}