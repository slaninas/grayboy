#pragma once

#include "cartridge.h"
#include "cpu.h"
#include "display.h"
#include "joypad.h"
#include "timer.h"
#include "fps.h"


inline auto format(const int& value, const uint32_t& width) -> std::string
{
	auto s = std::stringstream{};
	s << std::setfill('0') << std::setw(width) << std::hex << value;
	auto result = s.str();

	return result;
}

template<bool headless>
class Emulator {
public:
	const uint64_t CPU_FREQUENCY = 4'194'304 / 4;
	const uint64_t CYCLES_PER_FRAME = CPU_FREQUENCY / 60;

	Emulator(const std::string& cartridge_path)
	{
		memory_ = Memory{Cartridge{cartridge_path}};

		const auto regs =
		  RegistersChanger{.AF = 0x01b0, .BC = 0x0013, .DE = 0x00d8, .HL = 0x014d, .PC = 0x0100, .SP = 0xfffe}.get(
		    Registers{});

		cpu_ = Cpu{regs};
	}

	~Emulator()
	{
		if constexpr (!headless) {
			SDL_Quit();
		}
	}

	auto run() -> void
	{
		auto frame_cycles = uint64_t{0};

		while (true) {
			const auto cycles = execute_next();
			display_.update(memory_, cycles);

			frame_cycles += cycles;

			if (frame_cycles >= CYCLES_PER_FRAME) {
				frame_cycles -= CYCLES_PER_FRAME;
				display_.render();
				fps_.next_frame();

				const auto joypad_update = joypad_.update(memory_.read(0xff00));
				if (joypad_update.quit) {
					break;
				}

				if (joypad_update.request_interupt) {
					memory_.direct_write(0xff0f, memory_.direct_read(0xff0f) | 0x16);
				}

				memory_.update_joypad(joypad_update.state);
			}
		}
	}

	auto execute_next() -> uint64_t
	{
		// save_debug();

		auto cycles = 0;

		if (cpu_.registers().read_halt()) {
			const auto interupt = check_interupts();
			if (interupt > 0) {
				cpu_.registers().set_halt(false);

				if (cpu_.registers().read_IME()) {
					cycles = handle_interupt(interupt);
				}
			}
			else {
				cycles = 1;
			}
		}
		else {
			// Interupts
			if (cpu_.registers().read_IME()) {
				check_handle_interupts();
			}

			cycles = cpu_.execute_next(memory_);

			const auto ff02 = memory_.read(0xff02);

			if (ff02 == 0x81) {
				const auto c = static_cast<char>(memory_.read(0xff01));
				std::cout << c;
				serial_link_ += c;
				memory_.write(0xff02, 0x80);
			}
		}

		timer_.update(memory_, cycles);
		total_cycles_ += cycles;
		return cycles;
	}

	auto execute_instructions(const uint64_t& count)
	{
		for (auto i = static_cast<uint64_t>(0); i < count; ++i) { execute_next(); }
	}

	auto get_serial_link() const -> const std::string&
	{
		return serial_link_;
	}

	// auto dump_memory(const std::string& filename) -> void {
	// raw_dump(memory_, filename);
	// }

	// auto get_cpu() -> Cpu& {
	// return cpu_;
	// }

	// auto get_cpu() const -> const Cpu& {
	// return cpu_;
	// }

private:
	auto check_handle_interupts() -> void
	{
		const auto interupt = check_interupts();
		if (interupt > 0) {
			handle_interupt(interupt);
		}
	}

	auto check_interupts() -> uint8_t
	{
		// Interupt Enable and Interupt Requested
		const auto IE = memory_.read(0xffff);
		const auto IR = memory_.read(0xff0f);

		if (IE & 0x1 && IR & 0x1) {
			// V-Blank
			return 0x1;
		}
		else if (IE & 0x2 && IR & 0x2) {
			// LCD Stat
			return 0x2;
		}
		else if (IE & 0x4 && IR & 0x4) {
			// Timer
			return 0x4;
		}
		else if (IE & 0x8 && IR & 0x8) {
			// Serial
			return 0x8;
		}
		else if (IE & 0x16 && IR & 0x16) {
			// Joypad
			return 0x16;
		}
		return 0x00;
	}

	auto handle_interupt(const uint8_t& bit) -> uint64_t
	{
		memory_.write(0xff0f, memory_.read(0xff0f) ^ bit);

		auto& regs = cpu_.registers();

		const auto PC = regs.read("PC");
		const auto SP = regs.read("SP");

		const auto return_address_high = static_cast<uint8_t>((PC & 0xff00) >> 8);
		const auto return_address_low = static_cast<uint8_t>(PC & 0x00ff);

		memory_.write(SP - 1, return_address_high);
		memory_.write(SP - 2, return_address_low);
		regs.write("SP", SP - 2);

		switch (bit) {
			case 0x1:
				regs.write("PC", 0x40);
				break;
			case 0x2:
				regs.write("PC", 0x48);
				break;
			case 0x4:
				regs.write("PC", 0x50);
				break;
			case 0x8:
				regs.write("PC", 0x58);
				break;
			case 0x16:
				regs.write("PC", 0x60);
				break;
		}

		return 5;
	}

	// binjbg format
	auto save_debug() -> void
	{
		debug_log << std::hex;
		const auto PC = cpu_.registers().read("PC");
		debug_log << "A:" << format(cpu_.registers().read("A"), 2) << ' ';
		debug_log << "F:";
		debug_log << (cpu_.registers().read_flag("Z") ? 'Z' : '-');
		debug_log << (cpu_.registers().read_flag("N") ? 'N' : '-');
		debug_log << (cpu_.registers().read_flag("H") ? 'H' : '-');
		debug_log << (cpu_.registers().read_flag("C") ? 'C' : '-');
		debug_log << ' ';
		debug_log << "BC:" << format(cpu_.registers().read("B"), 2) << format(cpu_.registers().read("C"), 2) << ' ';
		debug_log << "DE:" << format(cpu_.registers().read("D"), 2) << format(cpu_.registers().read("E"), 2) << ' ';
		debug_log << "HL:" << format(cpu_.registers().read("H"), 2) << format(cpu_.registers().read("L"), 2) << ' ';
		debug_log << "SP:" << format(cpu_.registers().read("SP"), 4) << ' ';
		debug_log << "PC:" << format(PC, 4) << ' ';
		debug_log << "(cy: " << std::dec << total_cycles_ * 4 << ") " << std::hex;
		debug_log << "ppu:+" << (memory_.read(0xff41) & 0x3);
		debug_log << "|[00]0x" << format(PC, 4) << ": ";

		const auto info = cpu_.disassemble_next(PC, memory_);

		for (auto i = size_t{0}; i < info.memory_representation.size(); ++i) {
			debug_log << format(info.memory_representation[i], 2) << ' ';
		}

		debug_log << "\t\t" << info.instruction.mnemonic << ' ';

		// const auto mem = cpu_.get_memory();

		// debug_log << " | 0xff04 [DIV]: " << format(mem.read(0xff04), 2);
		// debug_log << " 0xff05 [TIMA]: " << format(mem.read(0xff05), 2);
		// debug_log << " 0xff06 [TMA]: " << format(mem.read(0xff06), 2);
		// debug_log << " 0xff07 [TAC]: " << format(mem.read(0xff07), 2);
		debug_log << '\n';
	}

	Cpu cpu_ = {};
	Memory memory_ = {};
	Joypad joypad_ = {};
	Display<headless> display_ = {};
	std::string serial_link_ = {};
	Timer timer_ = {};
	Fps fps_ = {};

	uint64_t total_cycles_ = {};
	std::ofstream debug_log = std::ofstream("debug_log");
};
