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

	};
}
