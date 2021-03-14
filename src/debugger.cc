// For debug
#include "cartridge.h"
#include "cpu.h"
#include "curse.h"
#include "registers.h"

#include <array>
#include <iostream>
#include <numeric>
#include <vector>

// TODO: Check why "06-ld r,r.gb" blarggs test ROM crash on different instruction when running "gameboy" vs "debugger" executable

void dprint(const DisassemblyInfo& info, std::ostream& os)
{
	os << std::hex;
	os << "0x" << std::setw(4) << std::setfill('0') << info.address << ": ";
	os << std::setw(15) << std::setfill(' ') << std::left << info.instruction.mnemonic  << " | ";
	for (const auto& val : info.memory_representation) { os << (int)val << ' '; }
	os << '\n';
	os << std::dec;
}

void dprint(const DisassemblyInfo& info)
{
	dprint(info, std::cout);
}

template<typename T>
void p(const T& cont)
{
	for (const auto el : cont) { dprint(el); }
}

template<typename T>
void print_memory(const T& mem)
{
	std::cout << std::hex;
	for (size_t i = 0; i < mem.size(); ++i) {
		const auto& value = mem[i];
		if (value != 0) { std::cout << "Memory 0x" << i << " = 0x" << (int)value << '\n'; }
	}
	std::cout << std::dec;
}

auto operator<<(std::ostream& os, const MemoryDiff& diff) -> std::ostream&
{
	os << std::hex;
	os << "[0x" << static_cast<int>(diff.address) << "] 0x" << static_cast<int>(diff.orig_value) << " -> 0x"
	   << static_cast<int>(diff.new_value);
	os << std::dec;
	return os;
}

auto operator<<(std::ostream& os, const std::vector<MemoryDiff>& diff) -> std::ostream&
{
	for (const auto& element : diff) { os << element << '\n'; }
	return os;
}

auto disassemble(Cpu& cpu)
{
	auto addr = static_cast<uint16_t>(cpu.registers().read("PC"));

	// TODO: Use something better than vector
	auto disassembled = std::vector<DisassemblyInfo>{};

	// TODO: Unify size of lookup with get_from_to
	for (int i = 0; i < 10; ++i) {
		const auto info = cpu.disassemble_next(addr);
		addr = info.next_address;
		disassembled.push_back(info);
	}

	std::sort(
	  begin(disassembled), end(disassembled), [](const auto& a, const auto& b) { return a.address < b.address; });
	const auto new_end = std::unique(
	  begin(disassembled), end(disassembled), [](const auto& a, const auto& b) { return a.address == b.address; });

	disassembled.erase(new_end, end(disassembled));
	return disassembled;
}

void update_instructions(
  const std::vector<DisassemblyInfo>& new_instructions,
  std::vector<DisassemblyInfo>& instructions)
{
	std::copy(
	  begin(new_instructions),
	  end(new_instructions),
	  std::back_insert_iterator<std::vector<DisassemblyInfo>>(instructions));

	std::sort(
	  begin(instructions), end(instructions), [](const auto& a, const auto& b) { return a.address < b.address; });
	auto new_end = std::unique(
	  begin(instructions), end(instructions), [](const auto& a, const auto& b) { return a.address == b.address; });

	instructions.erase(new_end, end(instructions));
}

auto get_from_to(
  const std::vector<DisassemblyInfo>& infos,
  const uint16_t& neighbors,
  const uint16_t& next_addr,
  std::ostream& os)
{
	auto next_addr_element =
	  std::find_if(begin(infos), end(infos), [next_addr](const auto& info) { return info.address == next_addr; });
	assert((next_addr_element != end(infos)) && "Well, well, it's broken. next_addr should already be in infos.");

	const auto start = begin(infos) < next_addr_element - neighbors ? next_addr_element - neighbors : begin(infos);
	// const auto last = end(infos) > next_addr_element + neighbors ? next_addr_element + neighbors : end(infos);
	const auto last = end(infos);

	for (auto iter = start; iter != last; ++iter) {
		const auto& info = *iter;
		auto character = info.address == next_addr ? '>' : ' ';
		os << character << ' ';
		dprint(info, os);
	}
}

auto main(int argc, const char** argv) -> int
{
	// const auto memory = MakeMemory{{{0x1234, 0x55}, {0x534, 0x21}, {0x0894, 0xab}}}.get();
	// const auto memory2 = MemoryChanger{{{0x1234, 0x56}, {0x5432, 0xfe}}}.get(memory);
	// print_memory(memory.dump());
	// std::cout << "--------------\n";
	// print_memory(memory2.dump());

	// const auto diff = memory_diff(memory, memory2);
	// std::cout << diff << '\n';

	// return 1;
	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " cartridge_filename\n";
		return 1;
	}

	auto filename = std::string(argv[1]);

	auto cart = Cartridge{filename};

	const auto rom = cart.dump();
	auto array = Memory::ArrayType{};
	// TODO: What to do with the memory at startup?
	std::iota(begin(array), end(array), 0);

	std::transform(begin(rom), end(rom), begin(array), [](const auto& el) { return static_cast<uint8_t>(el); });

	// Initializing values same way bgb emualtor does it
	const auto regs =
		// RegistersChanger{.AF = 0x1180, .DE = 0xff56, .HL = 0x000d, .PC = 0x0100, .SP = 0xfffe}.get(Registers{});
		// TODO: Use .PC = 0x0100 once the emulator works for the boot rom
	  RegistersChanger{.AF = 0x1180, .DE = 0xff56, .HL = 0x000d, .PC = 0x0000, .SP = 0xfffe}.get(Registers{});

	auto cpu = Cpu{array, regs};
	cpu.registers().print();

	auto next_addr = cpu.registers().read("PC");

	auto cursed = MainCurse{};
	auto instruction_window = CursedWindow{{0, 0}, {40, 50}};
	auto reg_window = CursedWindow{{40, 0}, {18, 13}};
	auto changes_window = CursedWindow{{40, 14}, {24, 13}};

	auto cpu_copy = cpu;
	auto disassembled_instructions = disassemble(cpu_copy);

	auto should_break = [](const auto& break_points, const auto& next_addr) {
		const auto iter = std::find(begin(break_points), end(break_points), next_addr);
		return iter != end(break_points);
	};

	// TODO: Fix: with blargg 06-ld r,r.gb test ROM, 0xc221 is correctly disasembled as LD SP, d16 but 0xc222 and 0xc223 are shown as nop,
	//       they should be part of the 0xc221 instruction
	// const auto break_points = std::vector<uint16_t>{
	// 0xc7f1,
	// 0xc7f9,
	// 0xc24f,
	// 0xc252}; // TODO: Compare with bgb, registers look ok to 0xc7f1 (including), memory not checked, diff at 0xc7f1
	const auto break_points = std::vector<uint16_t>{0x000c};

	auto running = false;
	// auto instruction_count = static_cast<uint64_t>(0);
	auto memory_snapshots = MemorySnapshots{cpu.get_memory()};

	while (true) {
		auto registers_ss = std::ostringstream{};
		auto instructions_stream = std::ostringstream{};
		auto memory_changes_stream = std::ostringstream{};

		const auto disassembled = cpu.disassemble_next(next_addr);
		next_addr = disassembled.next_address;

		cpu.registers().print(registers_ss);

		memory_changes_stream << "Last memory update:\n";
		const auto mem_diffs = memory_snapshots.getLastNonEmptyDiffs(10);
		memory_changes_stream << mem_diffs << '\n';

		const auto PC = cpu.registers().read("PC");
		get_from_to(disassembled_instructions, 10, PC, instructions_stream);
		if (!running) {
			instruction_window.update(instructions_stream.str());
			registers_ss << "IME: " << cpu.registers().read_IME() << '\n';
			reg_window.update(registers_ss.str());
			changes_window.update(memory_changes_stream.str());

			const auto c = cursed.get_char();
			if (c == 'r') { running = true; }
		}
		else {
			if (should_break(break_points, next_addr)) { running = false; }
		}

		[[maybe_unused]] const auto cycles = cpu.execute_next();
		cpu_copy = cpu;
		auto disassembled_new = disassemble(cpu_copy);
		update_instructions(disassembled_new, disassembled_instructions);
		memory_snapshots.add(cpu.get_memory());
	}
}
