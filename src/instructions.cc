#include "instructions.h"

auto get_8bit_instructions() -> std::vector<Instruction>;
auto get_16bit_instructions() -> std::vector<Instruction>;

auto get_all_instructions() -> std::vector<Instruction>
{
	auto instructions_8bit = get_8bit_instructions();
	const auto instructions_16bit = get_16bit_instructions();

	std::copy(begin(instructions_16bit), end(instructions_16bit), std::back_inserter(instructions_8bit));

	auto sorted = std::vector<Instruction>(256 * 2);
	for (const auto& orig_instr : instructions_8bit) {
		const auto opcode = orig_instr.opcode <= 0xff ? orig_instr.opcode : (orig_instr.opcode & 0xff) + 0x100;
		auto& instr = sorted[opcode];
		instr.mnemonic = orig_instr.mnemonic;
		instr.opcode = orig_instr.opcode;
		instr.size = orig_instr.size;
	}

	return sorted;
}

auto get_8bit_instructions() -> std::vector<Instruction>
{
	return std::vector<Instruction>{
	  {"NOP", 0x00, 1},          {"LD BC, d16", 0x01, 3},  {"LD DE, d16", 0x11, 3},   {"LD HL, d16", 0x21, 3},
	  {"LD SP, d16", 0x31, 3},   {"LD (BC), A", 0x02, 1},  {"LD (DE), A", 0x12, 1},   {"LD (HL+), A", 0x22, 1},
	  {"LD (HL-), A", 0x32, 1},  {"INC BC", 0x03, 1},      {"INC DE", 0x13, 1},       {"INC HL", 0x23, 1},
	  {"INC SP", 0x33, 1},       {"INC B", 0x04, 1},       {"INC C", 0x0c, 1},        {"INC E", 0x1c, 1},
	  {"INC L", 0x2c, 1},        {"INC A", 0x3c, 1},       {"INC D", 0x14, 1},        {"INC H", 0x24, 1},
	  {"INC (HL)", 0x34, 1},     {"DEC B", 0x05, 1},       {"DEC C", 0x0d, 1},        {"DEC E", 0x1d, 1},
	  {"DEC L", 0x2d, 1},        {"DEC A", 0x3d, 1},       {"DEC D", 0x15, 1},        {"DEC H", 0x25, 1},
	  {"DEC (HL)", 0x35, 1},     {"LD B, d8", 0x06, 2},    {"LD D, d8", 0x16, 2},     {"LD H, d8", 0x26, 2},
	  {"LD (HL), d8", 0x36, 2},  {"LD A, (BC)", 0x0a, 1},  {"LD A, (DE)", 0x1a, 1},   {"LD A, (HL+)", 0x2a, 1},
	  {"LD A, (HL-)", 0x3a, 1},  {"LD C, d8", 0x0e, 2},    {"LD E, d8", 0x1e, 2},     {"LD L, d8", 0x2e, 2},
	  {"LD A, d8", 0x3e, 2},     {"LD B, B", 0x40, 1},     {"LD B, C", 0x41, 1},      {"LD B, D", 0x42, 1},
	  {"LD B, E", 0x43, 1},      {"LD B, H", 0x44, 1},     {"LD B, L", 0x45, 1},      {"LD B, (HL)", 0x46, 1},
	  {"LD B, A", 0x47, 1},      {"LD C, B", 0x48, 1},     {"LD C, C", 0x49, 1},      {"LD C, D", 0x4a, 1},
	  {"LD C, E", 0x4b, 1},      {"LD C, H", 0x4c, 1},     {"LD C, L", 0x4d, 1},      {"LD C, (HL)", 0x4e, 1},
	  {"LD C, A", 0x4f, 1},      {"LD D, B", 0x50, 1},     {"LD D, C", 0x51, 1},      {"LD D, D", 0x52, 1},
	  {"LD D, E", 0x53, 1},      {"LD D, H", 0x54, 1},     {"LD D, L", 0x55, 1},      {"LD D, (HL)", 0x56, 1},
	  {"LD D, A", 0x57, 1},      {"LD E, B", 0x58, 1},     {"LD E, C", 0x59, 1},      {"LD E, D", 0x5a, 1},
	  {"LD E, E", 0x5b, 1},      {"LD E, H", 0x5c, 1},     {"LD E, L", 0x5d, 1},      {"LD E, (HL)", 0x5e, 1},
	  {"LD E, A", 0x5f, 1},      {"LD H, B", 0x60, 1},     {"LD H, C", 0x61, 1},      {"LD H, D", 0x62, 1},
	  {"LD H, E", 0x63, 1},      {"LD H, H", 0x64, 1},     {"LD H, L", 0x65, 1},      {"LD H, (HL)", 0x66, 1},
	  {"LD H, A", 0x67, 1},      {"LD L, B", 0x68, 1},     {"LD L, C", 0x69, 1},      {"LD L, D", 0x6a, 1},
	  {"LD L, E", 0x6b, 1},      {"LD L, H", 0x6c, 1},     {"LD L, L", 0x6d, 1},      {"LD L, (HL)", 0x6e, 1},
	  {"LD L, A", 0x6f, 1},      {"LD (HL), B", 0x70, 1},  {"LD (HL), C", 0x71, 1},   {"LD (HL), D", 0x72, 1},
	  {"LD (HL), E", 0x73, 1},   {"LD (HL), H", 0x74, 1},  {"LD (HL), L", 0x75, 1},   {"LD (HL), A", 0x77, 1},
	  {"LD A, B", 0x78, 1},      {"LD A, C", 0x79, 1},     {"LD A, D", 0x7a, 1},      {"LD A, E", 0x7b, 1},
	  {"LD A, H", 0x7c, 1},      {"LD A, L", 0x7d, 1},     {"LD A, (HL)", 0x7e, 1},   {"LD A, A", 0x7f, 1},
	  {"LD (a8), A", 0xe0, 2},   {"LD A, (a8)", 0xf0, 2},  {"LD (C), A", 0xe2, 1},    {"LD A, (C)", 0xf2, 1},
	  {"POP BC", 0xc1, 1},       {"POP DE", 0xd1, 1},      {"POP HL", 0xe1, 1},       {"POP AF", 0xf1, 1},
	  {"PUSH BC", 0xc5, 1},      {"PUSH DE", 0xd5, 1},     {"PUSH HL", 0xe5, 1},      {"PUSH AF", 0xf5, 1},
	  {"LD HL, SP+s8", 0xf8, 2}, {"LD SP, HL", 0xf9, 1},   {"DEC BC", 0x0b, 1},       {"DEC DE", 0x1b, 1},
	  {"DEC HL", 0x2b, 1},       {"DEC SP", 0x3b, 1},      {"RLCA", 0x07, 1},         {"RLA", 0x17, 1},
	  {"RRCA", 0x0f, 1},         {"RRA", 0x1f, 1},         {"LD (a16), SP", 0x08, 3}, {"LD (a16), A", 0xea, 3},
	  {"LD A, (a16)", 0xfa, 3},  {"DAA", 0x27, 1},         {"SCF", 0x37, 1},          {"HALT", 0x76, 1},
	  {"CPL", 0x2f, 1},          {"CCF", 0x3f, 1},         {"DI", 0xf3, 1},           {"EI", 0xfb, 1},
	  {"ADD HL, BC", 0x09, 1},   {"ADD HL, DE", 0x19, 1},  {"ADD HL, HL", 0x29, 1},   {"ADD HL, SP", 0x39, 1},
	  {"ADD A, B", 0x80, 1},     {"ADD A, C", 0x81, 1},    {"ADD A, D", 0x82, 1},     {"ADD A, E", 0x83, 1},
	  {"ADD A, H", 0x84, 1},     {"ADD A, L", 0x85, 1},    {"ADD A, (HL)", 0x86, 1},  {"ADD A, L", 0x87, 1},
	  {"ADC A, B", 0x88, 1},     {"ADC A, C", 0x89, 1},    {"ADC A, D", 0x8a, 1},     {"ADC A, E", 0x8b, 1},
	  {"ADC A, H", 0x8c, 1},     {"ADC A, L", 0x8d, 1},    {"ADC A, (HL)", 0x8e, 1},  {"ADC A, A", 0x8f, 1},
	  {"ADC A, d8", 0xce, 2},    {"SUB B", 0x90, 1},       {"SUB C", 0x91, 1},        {"SUB D", 0x92, 1},
	  {"SUB E", 0x93, 1},        {"SUB H", 0x94, 1},       {"SUB L", 0x95, 1},        {"SUB (HL)", 0x96, 1},
	  {"SUB A", 0x97, 1},        {"SBC A, B", 0x98, 1},    {"SBC A, C", 0x99, 1},     {"SBC A, D", 0x9a, 1},
	  {"SBC A, E", 0x9b, 1},     {"SBC A, H", 0x9c, 1},    {"SBC A, L", 0x9d, 1},     {"SBC A, (HL)", 0x9e, 1},
	  {"SBC A, A", 0x9f, 1},     {"SBC A, d8", 0xde, 2},   {"AND B", 0xa0, 1},        {"AND C", 0xa1, 1},
	  {"AND D", 0xa2, 1},        {"AND E", 0xa3, 1},       {"AND H", 0xa4, 1},        {"AND L", 0xa5, 1},
	  {"AND (HL)", 0xa6, 1},     {"AND A", 0xa7, 1},       {"XOR B", 0xa8, 1},        {"XOR C", 0xa9, 1},
	  {"XOR D", 0xaa, 1},        {"XOR E", 0xab, 1},       {"XOR H", 0xac, 1},        {"XOR L", 0xad, 1},
	  {"XOR (HL)", 0xae, 1},     {"XOR A", 0xaf, 1},       {"XOR d8", 0xee, 2},       {"OR B", 0xb0, 1},
	  {"OR C", 0xb1, 1},         {"OR D", 0xb2, 1},        {"OR E", 0xb3, 1},         {"OR H", 0xb4, 1},
	  {"OR L", 0xb5, 1},         {"OR (HL)", 0xb6, 1},     {"OR A", 0xb7, 1},         {"CP B", 0xb8, 1},
	  {"CP C", 0xb9, 1},         {"CP D", 0xba, 1},        {"CP E", 0xbb, 1},         {"CP H", 0xbc, 1},
	  {"CP L", 0xbd, 1},         {"CP (HL)", 0xbe, 1},     {"CP A", 0xbf, 1},         {"CP d8", 0xfe, 2},
	  {"ADD A, d8", 0xc6, 2},    {"SUB A, d8", 0xd6, 2},   {"AND d8", 0xe6, 2},       {"OR d8", 0xf6, 2},
	  {"ADD SP, s8", 0xe8, 2},   {"JR s8", 0x18, 2},       {"JR, Z s8", 0x28, 2},     {"JR, C s8", 0x38, 2},
	  {"RET NZ", 0xc0, 1},       {"RET NC", 0xd0, 1},      {"RET Z", 0xc8, 1},        {"RET C", 0xd8, 1},
	  {"RETI", 0xd9, 1},         {"RET", 0xc9, 1},         {"JP NZ, a16", 0xc2, 3},   {"JP Z, a16", 0xca, 3},
	  {"JP NC, a16", 0xd2, 3},   {"JP C, a16", 0xda, 3},   {"JP a16", 0xc3, 3},       {"JP (HL)", 0xe9, 1},
	  {"CALL NZ, a16", 0xc4, 3}, {"CALL Z, a16", 0xcc, 3}, {"CALL NC, a16", 0xd4, 3}, {"CALL C, a16", 0xdc, 3},
	  {"CALL a16", 0xcd, 3},     {"RST 0", 0xc7, 1},       {"RST 2", 0xd7, 1},        {"RST 4", 0xe7, 1},
	  {"RST 6", 0xf7, 1},        {"RST 1", 0xcf, 1},       {"RST 3", 0xdf, 1},        {"RST 5", 0xef, 1},
	  {"RST 7", 0xff, 1},        {"JR NZ, s8", 0x20, 2},   {"JR NC, s8", 0x30, 2},
	};
}

auto get_16bit_instructions() -> std::vector<Instruction>
{
	return std::vector<Instruction>{
	  {"RLC B", 0xcb00, 2},    {"RLC C", 0xcb01, 2},    {"RLC D", 0xcb02, 2},       {"RLC E", 0xcb03, 2},
	  {"RLC H", 0xcb04, 2},    {"RLC L", 0xcb05, 2},    {"RLC (HL)", 0xcb06, 2},    {"RLC A", 0xcb07, 2},
	  {"RRC B", 0xcb08, 2},    {"RRC C", 0xcb09, 2},    {"RRC D", 0xcb0a, 2},       {"RRC E", 0xcb0b, 2},
	  {"RRC H", 0xcb0c, 2},    {"RRC L", 0xcb0d, 2},    {"RRC (HL)", 0xcb0e, 2},    {"RRC A", 0xcb0f, 2},
	  {"RL B", 0xcb10, 2},     {"RL C", 0xcb11, 2},     {"RL D", 0xcb12, 2},        {"RL E", 0xcb13, 2},
	  {"RL H", 0xcb14, 2},     {"RL L", 0xcb15, 2},     {"RL (HL)", 0xcb16, 2},     {"RL A", 0xcb17, 2},
	  {"RR B", 0xcb18, 2},     {"RR C", 0xcb19, 2},     {"RR D", 0xcb1a, 2},        {"RR E", 0xcb1b, 2},
	  {"RR H", 0xcb1c, 2},     {"RR L", 0xcb1d, 2},     {"RR (HL)", 0xcb1e, 2},     {"RR A", 0xcb1f, 2},
	  {"SLA B", 0xcb20, 2},    {"SLA C", 0xcb21, 2},    {"SLA D", 0xcb22, 2},       {"SLA E", 0xcb23, 2},
	  {"SLA H", 0xcb24, 2},    {"SLA L", 0xcb25, 2},    {"SLA (HL)", 0xcb26, 2},    {"SLA A", 0xcb27, 2},
	  {"SRA B", 0xcb28, 2},    {"SRA C", 0xcb29, 2},    {"SRA D", 0xcb2a, 2},       {"SRA E", 0xcb2b, 2},
	  {"SRA H", 0xcb2c, 2},    {"SRA L", 0xcb2d, 2},    {"SRA (HL)", 0xcb2e, 2},    {"SRA A", 0xcb2f, 2},
	  {"SWAP B", 0xcb30, 2},   {"SWAP C", 0xcb31, 2},   {"SWAP D", 0xcb32, 2},      {"SWAP E", 0xcb33, 2},
	  {"SWAP H", 0xcb34, 2},   {"SWAP L", 0xcb35, 2},   {"SWAP (HL)", 0xcb36, 2},   {"SWAP A", 0xcb37, 2},
	  {"SRL B", 0xcb38, 2},    {"SRL C", 0xcb39, 2},    {"SRL D", 0xcb3a, 2},       {"SRL E", 0xcb3b, 2},
	  {"SRL H", 0xcb3c, 2},    {"SRL L", 0xcb3d, 2},    {"SRL (HL)", 0xcb3e, 2},    {"SRL A", 0xcb3f, 2},
	  {"BIT 0, B", 0xcb40, 2}, {"BIT 0, C", 0xcb41, 2}, {"BIT 0, D", 0xcb42, 2},    {"BIT 0, E", 0xcb43, 2},
	  {"BIT 0, H", 0xcb44, 2}, {"BIT 0, L", 0xcb45, 2}, {"BIT 0, (HL)", 0xcb46, 2}, {"BIT 0, A", 0xcb47, 2},
	  {"BIT 1, B", 0xcb48, 2}, {"BIT 1, C", 0xcb49, 2}, {"BIT 1, D", 0xcb4a, 2},    {"BIT 1, E", 0xcb4b, 2},
	  {"BIT 1, H", 0xcb4c, 2}, {"BIT 1, L", 0xcb4d, 2}, {"BIT 1, (HL)", 0xcb4e, 2}, {"BIT 1, A", 0xcb4f, 2},
	  {"BIT 2, B", 0xcb50, 2}, {"BIT 2, C", 0xcb51, 2}, {"BIT 2, D", 0xcb52, 2},    {"BIT 2, E", 0xcb53, 2},
	  {"BIT 2, H", 0xcb54, 2}, {"BIT 2, L", 0xcb55, 2}, {"BIT 2, (HL)", 0xcb56, 2}, {"BIT 2, A", 0xcb57, 2},
	  {"BIT 3, B", 0xcb58, 2}, {"BIT 3, C", 0xcb59, 2}, {"BIT 3, D", 0xcb5a, 2},    {"BIT 3, E", 0xcb5b, 2},
	  {"BIT 3, H", 0xcb5c, 2}, {"BIT 3, L", 0xcb5d, 2}, {"BIT 3, (HL)", 0xcb5e, 2}, {"BIT 3, A", 0xcb5f, 2},
	  {"BIT 4, B", 0xcb60, 2}, {"BIT 4, C", 0xcb61, 2}, {"BIT 4, D", 0xcb62, 2},    {"BIT 4, E", 0xcb63, 2},
	  {"BIT 4, H", 0xcb64, 2}, {"BIT 4, L", 0xcb65, 2}, {"BIT 4, (HL)", 0xcb66, 2}, {"BIT 4, A", 0xcb67, 2},
	  {"BIT 5, B", 0xcb68, 2}, {"BIT 5, C", 0xcb69, 2}, {"BIT 5, D", 0xcb6a, 2},    {"BIT 5, E", 0xcb6b, 2},
	  {"BIT 5, H", 0xcb6c, 2}, {"BIT 5, L", 0xcb6d, 2}, {"BIT 5, (HL)", 0xcb6e, 2}, {"BIT 5, A", 0xcb6f, 2},
	  {"BIT 6, B", 0xcb70, 2}, {"BIT 6, C", 0xcb71, 2}, {"BIT 6, D", 0xcb72, 2},    {"BIT 6, E", 0xcb73, 2},
	  {"BIT 6, H", 0xcb74, 2}, {"BIT 6, L", 0xcb75, 2}, {"BIT 6, (HL)", 0xcb76, 2}, {"BIT 6, A", 0xcb77, 2},
	  {"BIT 7, B", 0xcb78, 2}, {"BIT 7, C", 0xcb79, 2}, {"BIT 7, D", 0xcb7a, 2},    {"BIT 7, E", 0xcb7b, 2},
	  {"BIT 7, H", 0xcb7c, 2}, {"BIT 7, L", 0xcb7d, 2}, {"BIT 7, (HL)", 0xcb7e, 2}, {"BIT 7, A", 0xcb7f, 2},
	  {"RES 0, B", 0xcb80, 2}, {"RES 0, C", 0xcb81, 2}, {"RES 0, D", 0xcb82, 2},    {"RES 0, E", 0xcb83, 2},
	  {"RES 0, H", 0xcb84, 2}, {"RES 0, L", 0xcb85, 2}, {"RES 0, (HL)", 0xcb86, 2}, {"RES 0, A", 0xcb87, 2},
	  {"RES 1, B", 0xcb88, 2}, {"RES 1, C", 0xcb89, 2}, {"RES 1, D", 0xcb8a, 2},    {"RES 1, E", 0xcb8b, 2},
	  {"RES 1, H", 0xcb8c, 2}, {"RES 1, L", 0xcb8d, 2}, {"RES 1, (HL)", 0xcb8e, 2}, {"RES 1, A", 0xcb8f, 2},
	  {"RES 2, B", 0xcb90, 2}, {"RES 2, C", 0xcb91, 2}, {"RES 2, D", 0xcb92, 2},    {"RES 2, E", 0xcb93, 2},
	  {"RES 2, H", 0xcb94, 2}, {"RES 2, L", 0xcb95, 2}, {"RES 2, (HL)", 0xcb96, 2}, {"RES 2, A", 0xcb97, 2},
	  {"RES 3, B", 0xcb98, 2}, {"RES 3, C", 0xcb99, 2}, {"RES 3, D", 0xcb9a, 2},    {"RES 3, E", 0xcb9b, 2},
	  {"RES 3, H", 0xcb9c, 2}, {"RES 3, L", 0xcb9d, 2}, {"RES 3, (HL)", 0xcb9e, 2}, {"RES 3, A", 0xcb9f, 2},
	  {"RES 4, B", 0xcba0, 2}, {"RES 4, C", 0xcba1, 2}, {"RES 4, D", 0xcba2, 2},    {"RES 4, E", 0xcba3, 2},
	  {"RES 4, H", 0xcba4, 2}, {"RES 4, L", 0xcba5, 2}, {"RES 4, (HL)", 0xcba6, 2}, {"RES 4, A", 0xcba7, 2},
	  {"RES 5, B", 0xcba8, 2}, {"RES 5, C", 0xcba9, 2}, {"RES 5, D", 0xcbaa, 2},    {"RES 5, E", 0xcbab, 2},
	  {"RES 5, H", 0xcbac, 2}, {"RES 5, L", 0xcbad, 2}, {"RES 5, (HL)", 0xcbae, 2}, {"RES 5, A", 0xcbaf, 2},
	  {"RES 6, B", 0xcbb0, 2}, {"RES 6, C", 0xcbb1, 2}, {"RES 6, D", 0xcbb2, 2},    {"RES 6, E", 0xcbb3, 2},
	  {"RES 6, H", 0xcbb4, 2}, {"RES 6, L", 0xcbb5, 2}, {"RES 6, (HL)", 0xcbb6, 2}, {"RES 6, A", 0xcbb7, 2},
	  {"RES 7, B", 0xcbb8, 2}, {"RES 7, C", 0xcbb9, 2}, {"RES 7, D", 0xcbba, 2},    {"RES 7, E", 0xcbbb, 2},
	  {"RES 7, H", 0xcbbc, 2}, {"RES 7, L", 0xcbbd, 2}, {"RES 7, (HL)", 0xcbbe, 2}, {"RES 7, A", 0xcbbf, 2},
	  {"SET 0, B", 0xcbc0, 2}, {"SET 0, C", 0xcbc1, 2}, {"SET 0, D", 0xcbc2, 2},    {"SET 0, E", 0xcbc3, 2},
	  {"SET 0, H", 0xcbc4, 2}, {"SET 0, L", 0xcbc5, 2}, {"SET 0, (HL)", 0xcbc6, 2}, {"SET 0, A", 0xcbc7, 2},
	  {"SET 1, B", 0xcbc8, 2}, {"SET 1, C", 0xcbc9, 2}, {"SET 1, D", 0xcbca, 2},    {"SET 1, E", 0xcbcb, 2},
	  {"SET 1, H", 0xcbcc, 2}, {"SET 1, L", 0xcbcd, 2}, {"SET 1, (HL)", 0xcbce, 2}, {"SET 1, A", 0xcbcf, 2},
	  {"SET 2, B", 0xcbd0, 2}, {"SET 2, C", 0xcbd1, 2}, {"SET 2, D", 0xcbd2, 2},    {"SET 2, E", 0xcbd3, 2},
	  {"SET 2, H", 0xcbd4, 2}, {"SET 2, L", 0xcbd5, 2}, {"SET 2, (HL)", 0xcbd6, 2}, {"SET 2, A", 0xcbd7, 2},
	  {"SET 3, B", 0xcbd8, 2}, {"SET 3, C", 0xcbd9, 2}, {"SET 3, D", 0xcbda, 2},    {"SET 3, E", 0xcbdb, 2},
	  {"SET 3, H", 0xcbdc, 2}, {"SET 3, L", 0xcbdd, 2}, {"SET 3, (HL)", 0xcbde, 2}, {"SET 3, A", 0xcbdf, 2},
	  {"SET 4, B", 0xcbe0, 2}, {"SET 4, C", 0xcbe1, 2}, {"SET 4, D", 0xcbe2, 2},    {"SET 4, E", 0xcbe3, 2},
	  {"SET 4, H", 0xcbe4, 2}, {"SET 4, L", 0xcbe5, 2}, {"SET 4, (HL)", 0xcbe6, 2}, {"SET 4, A", 0xcbe7, 2},
	  {"SET 5, B", 0xcbe8, 2}, {"SET 5, C", 0xcbe9, 2}, {"SET 5, D", 0xcbea, 2},    {"SET 5, E", 0xcbeb, 2},
	  {"SET 5, H", 0xcbec, 2}, {"SET 5, L", 0xcbed, 2}, {"SET 5, (HL)", 0xcbee, 2}, {"SET 5, A", 0xcbef, 2},
	  {"SET 6, B", 0xcbf0, 2}, {"SET 6, C", 0xcbf1, 2}, {"SET 6, D", 0xcbf2, 2},    {"SET 6, E", 0xcbf3, 2},
	  {"SET 6, H", 0xcbf4, 2}, {"SET 6, L", 0xcbf5, 2}, {"SET 6, (HL)", 0xcbf6, 2}, {"SET 6, A", 0xcbf7, 2},
	  {"SET 7, B", 0xcbf8, 2}, {"SET 7, C", 0xcbf9, 2}, {"SET 7, D", 0xcbfa, 2},    {"SET 7, E", 0xcbfb, 2},
	  {"SET 7, H", 0xcbfc, 2}, {"SET 7, L", 0xcbfd, 2}, {"SET 7, (HL)", 0xcbfe, 2}, {"SET 7, A", 0xcbff, 2},
	};
}
