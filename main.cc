// For debug
#include <iostream>
#include <vector>
#include <array>

#include "cartridge.h"
#include "cpu.h"
#include "registers.h"
#include "registers.h"
#include "curse.h"

void dprint(const DisassemblyInfo& info, std::ostream& os) {
	os << std::hex;
	os << "0x" << std::setw(4) << std::setfill('0') << info.address << ": " << info.instruction.mnemonic << " | ";
	for (const auto& val : info.memory_representation) {
		os << (int)val << ' ';
	}
	os << '\n';
	os << std::dec;
}

void dprint(const DisassemblyInfo& info) {
	dprint(info, std::cout);
}


template<typename T>
void p(const T& cont) {
	for (const auto el : cont) {
		dprint(el);
	}
}

template<typename T>
void print_memory(const T& mem) {

	for (int i = 0; i < mem.size(); ++i) {
		const auto& value = mem[i];
		if (value != 0) {
			std::cout << "Memory 0x" << i << "=" << (int)value <<'\n';
		}
	}
}

auto disassemble(Cpu& cpu) {
	auto addr = static_cast<uint16_t>(cpu.registers().read("PC"));

	// TODO: Use something better than vector
	auto disassembled = std::vector<DisassemblyInfo>{};

	// TODO: Unify size of lookup with get_from_to
	for (int i = 0; i < 10; ++i) {
		const auto info =  cpu.disassemble_next(addr);
		addr = info.next_address;
		disassembled.push_back(info);
	}

	std::sort(
		begin(disassembled),
		end(disassembled),
		[](const auto& a, const auto& b) {
			return a.address < b.address;
		}
	);
	auto new_end = std::unique(begin(disassembled), end(disassembled), [] (const auto& a, const auto&b) { return a.address == b.address; } );

	disassembled.erase(new_end, end(disassembled));
	return disassembled;
}

void update_instructions(const std::vector<DisassemblyInfo>& new_instructions, std::vector<DisassemblyInfo>& instructions) {
	std::copy(begin(new_instructions), end(new_instructions), std::back_insert_iterator<std::vector<DisassemblyInfo>>(instructions));

	std::sort(
		begin(instructions),
		end(instructions),
		[](const auto& a, const auto& b) {
			return a.address < b.address;
		}
	);
	auto new_end = std::unique(begin(instructions), end(instructions), [] (const auto& a, const auto&b) { return a.address == b.address; } );

	instructions.erase(new_end, end(instructions));
}

auto get_from_to(const std::vector<DisassemblyInfo>& infos, const uint16_t& neighbors, const uint16_t& next_addr, std::ostream& os) {

	auto next_addr_element = std::find_if(begin(infos), end(infos), [next_addr](const auto& info) { return info.address == next_addr; });
	assert((next_addr_element != end(infos)) && "Well, well, it's broken. next_addr should already be in infos.");

	const auto start = begin(infos) < next_addr_element - neighbors ? next_addr_element - neighbors : begin(infos);
	const auto last = end(infos) > next_addr_element + neighbors ? next_addr_element + neighbors : end(infos);


	// std::cout << "start: " << start << ", end: " << end << '\n';

	for (auto iter = start; iter != last; ++iter) {
		const auto& info = *iter;
		auto character = info.address == next_addr ? '>' : ' ';
		os << character << ' ';
		dprint(info, os);
	}
}

int main(int argc, const char** argv) {
	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " cartridge_filename\n";
		return 1;
	}

	auto filename = std::string(argv[1]);

	auto cart = Cartridge{filename};
	// cart.print_info();

	const auto rom = cart.dump();
	auto array = Memory::ArrayType{};

	std::transform(
		begin(rom),
		end(rom),
		begin(array),
		[](const auto& el) { return static_cast<uint8_t>(el); }
	);

	// Initializing values same way bgb emualtor does it
	const auto regs = RegistersChanger{.AF=0x1180, .DE=0xff56, .HL=0x000d, .PC=0x0100, .SP=0xfffe}.get(Registers{});

	auto cpu = Cpu{array, regs};
	cpu.registers().print();

	auto next_addr = cpu.registers().read("PC");

	auto cursed = MainCurse{};
	auto reg_window = CursedWindow{{40, 10}, {18, 12}};
	auto instruction_window = CursedWindow{{0, 0}, {40, 50}};

	auto cpu_copy = cpu;
	auto disassembled_instructions = disassemble(cpu_copy);


	while (1) {
		auto registers_ss = std::ostringstream{};
		auto ss = std::ostringstream{};

		const auto disassembled = cpu.disassemble_next(next_addr);
		next_addr = disassembled.next_address;

		cpu.registers().print(registers_ss);

		reg_window.update(registers_ss.str());

		const auto PC = cpu.registers().read("PC");
		get_from_to(disassembled_instructions, 10, PC, ss);
		instruction_window.update(ss.str());

		cursed.wait_for_any();

		[[maybe_unused]] const auto cycles = cpu.execute_next();
		cpu_copy = cpu;
		auto disassembled_new = disassemble(cpu_copy);
		update_instructions(disassembled_new, disassembled_instructions);
	}

}
