#pragma once

class Timer {
public:
	const uint64_t CPU_FREQUENCY = 4'194'304 / 4;
	const uint64_t DIV_REGISTER_FREQUENCY = 16'384;
	const uint64_t DIV_REGISTER_CYCLES_PER_UPDATE = CPU_FREQUENCY / DIV_REGISTER_FREQUENCY;

	// Inspiration: http://emudev.de/gameboy-emulator/interrupts-and-timers/
	auto update(Memory& memory, const uint64_t& new_cycles)
	{
		div_register_cycles_ += new_cycles;

		if (div_register_cycles_ > DIV_REGISTER_CYCLES_PER_UPDATE) {
			div_register_cycles_ -= DIV_REGISTER_CYCLES_PER_UPDATE;
			memory.direct_write(0xff04, memory.read(0xff04) + 1);
		}

		const auto TAC = memory.read(0xff07);
		if (TAC & (1 << 2)) {
			timer_counter_cycles_ += new_cycles;

			auto frequency = 4096;

			if ((TAC & 0x3) == 0x0) { frequency = frequency * 1; }
			else if ((TAC & 0x3) == 0x1) {
				frequency = frequency * 64;
			}
			else if ((TAC & 0x3) == 0x2) {
				frequency = frequency * 16;
			}
			else if ((TAC & 0x3) == 0x3) {
				frequency = frequency * 4;
			}

			const auto timer_counter_cycles_per_update = CPU_FREQUENCY / frequency;
			while (timer_counter_cycles_ >= timer_counter_cycles_per_update) {
				memory.write(0xff05, memory.read(0xff05) + 1);

				if (memory.read(0xff05) == 0x00) {
					memory.write(0xff0f, memory.read(0xff0f) | (1 << 2));

					const auto TMA = memory.read(0xff06);
					memory.write(0xff05, TMA);
				}

				timer_counter_cycles_ -= timer_counter_cycles_per_update;
			}
		}
	}

private:
	uint64_t div_register_cycles_ = {};
	uint64_t timer_counter_cycles_ = {};
};
