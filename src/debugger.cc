// For debug
#include "display.h"
#include "emulator.h"
#include "curse.h"
#include "registers.h"

#include <array>
#include <iostream>
#include <numeric>
#include <vector>

// TODO: Check why "06-ld r,r.gb" blarggs test ROM crash on different instruction when running "grayboy" vs "debugger" executable

void dprint(const DisassemblyInfo& info, std::ostream& os)
{
	os << std::hex;
	os << "0x" << std::setw(4) << std::setfill('0') << std::right << 	info.address << ": " << std::flush;
	os << std::setw(15) << std::setfill(' ') << std::left << info.instruction.mnemonic  << " | ";
	for (const auto& val : info.memory_representation) { os << std::setfill('0') << std::right << std::setw(2) << (int)val << ' '; }
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

// TODO: Simplify - read X instruction but do not actually follow jumps
auto disassemble(const Cpu& cpu)
{
	auto addr = static_cast<uint16_t>(cpu.registers().read("PC"));

	// TODO: Use something better than vector
	auto disassembled = std::vector<DisassemblyInfo>{};

	// TODO: Unify size of lookup with get_from_to
	for (int i = 0; i < 10; ++i) {
		auto info = DisassemblyInfo{};
		try {
			info = cpu.disassemble_next(addr);
			addr += info.memory_representation.size();
		} catch (const std::runtime_error& e) {
			info = DisassemblyInfo{addr, static_cast<uint16_t>(addr + 1), Instruction{}, std::vector{cpu.get_memory().read(addr)}};
			addr = addr + 1;
		}
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
	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << "cartridge_filename\n";
		return 1;
	}

	auto emu = Emulator{argv[1]};
	// auto display = Display{};

	auto next_addr = emu.get_cpu().registers().read("PC");

	auto cursed = MainCurse{};
	auto instruction_window = CursedWindow{{0, 0}, {40, 50}};
	auto reg_window = CursedWindow{{40, 0}, {18, 13}};
	auto changes_window = CursedWindow{{40, 26}, {24, 13}};
	auto reg_changes_window = CursedWindow{{40, 13}, {24, 13}};

	auto cpu_copy = emu.get_cpu();
	auto disassembled_instructions = disassemble(cpu_copy);

	auto should_break = [](const auto& break_points, const auto& next_addr) {
		const auto iter = std::find(begin(break_points), end(break_points), next_addr);
		return iter != end(break_points);
	};

	const auto break_points = std::vector<uint16_t>{
		0x0048};

	auto running = false;
	// auto instruction_count = static_cast<uint64_t>(0);
	auto memory_snapshots = MemorySnapshots{emu.get_cpu().get_memory()};
	auto registers_snapshot = RegistersSnaphost{emu.get_cpu().registers()};
	registers_snapshot.add(emu.get_cpu().registers());

	auto f = std::ofstream("f");

	while (true) {
		// const auto cycles = emu.execute_next();
		// if (!display.update(emu.get_memory(), cycles)) {
			// return 0;
		// }

	// }
	// while (false){
		if (!running) {

			auto registers_stream = std::ostringstream{};
			auto instructions_stream = std::ostringstream{};
			auto memory_changes_stream = std::ostringstream{};
			auto reg_changes_stream = std::ostringstream{};

			const auto disassembled = emu.get_cpu().disassemble_next(next_addr);
			next_addr = disassembled.next_address;

			emu.get_cpu().registers().print(registers_stream);

			memory_changes_stream << "Last memory update:\n";
			const auto mem_diffs = memory_snapshots.getLastNonEmptyDiffs(10);
			memory_changes_stream << mem_diffs << '\n';

			reg_changes_stream << "Last register changes:\n";
			// reg_changes_stream << registers_diff(registers_snapshot.get(1), emu.get_cpu().registers());

			const auto PC = emu.get_cpu().registers().read("PC");
			get_from_to(disassembled_instructions, 10, PC, instructions_stream);

			instruction_window.update(instructions_stream.str());
			registers_stream << "IME: " << emu.get_cpu().registers().read_IME() << '\n';
			reg_window.update(registers_stream.str());
			changes_window.update(memory_changes_stream.str());
			reg_changes_window.update(reg_changes_stream.str());


			const auto c = cursed.get_char();
			switch (c) {
				case 'r':
					running = true;
					break;
				case 'm':
					raw_dump(emu.get_cpu().memory_dump(), "memory_dump");
					continue;
				default: break;
			}
		}
		else {
			if (should_break(break_points, next_addr)) { running = false; }
		}

		[[maybe_unused]] const auto cycles = emu.execute_next();
		// if (!display.update(emu.get_memory(), cycles)) {
			// return 0;
		// }
		cpu_copy = emu.get_cpu();
		auto disassembled_new = disassemble(cpu_copy);
		update_instructions(disassembled_new, disassembled_instructions);
		// TODO: Speed up? memory_snapshots.add() is causing massive slowdown when running to the breakpoint (debug build only)
		// memory_snapshots.add(emu.get_cpu().get_memory());
		// registers_snapshot.add(emu.get_cpu().registers());
	}
}
