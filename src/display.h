#pragma once
#include <SDL2/SDL.h>
#include "memory.h"


// Useful sources:
// - https://stackoverflow.com/a/35989490/1112468
// - http://emudev.de/gameboy-emulator/%e2%af%88-ppu-rgb-arrays-and-sdl/
// - http://www.codeslinger.co.uk/pages/projects/gameboy.html
class Display {
public:

	const uint64_t CYCLES_PER_SCANLINE = 456 / 4;

	Display() {
		 SDL_Init(SDL_INIT_VIDEO);
		 SDL_CreateWindowAndRenderer(160 * 5, 144 * 5, 0, &window_, &renderer_);
		 SDL_RenderSetScale(renderer_, 5.0, 5.0);
	}

	auto update(Memory& mem, const uint16_t& cycles) {
		// TODO: Don't do it every update
		scanline_cycles_ += cycles;

		const auto palette = mem.read(0xff47);
		const auto colors = std::array{palette & 0x3, (palette & 0xc) >> 2, (palette & 0x30) >> 4, (palette & 0xc0) >> 6};

		const auto SCY = mem.read(0xff42);
		const auto SCX = mem.read(0xff43);

		const auto scanline = mem.read(0xff44);
		if (scanline == 0) {
			update_map(mem);
		}

		if (scanline_cycles_ > CYCLES_PER_SCANLINE) {
			scanline_cycles_ -= CYCLES_PER_SCANLINE;


			if (scanline >= 0x99) {
				mem.write(0xff44, 0);
				return;
			}

			if (scanline < 0x90) {

				for (auto x = 0; x < 160; ++x) {
					display_[x][scanline] = colors[buffer_[(x + SCX) % 256][(scanline + SCY) % 256]];
				}
			}

			mem.write(0xff44, scanline + 1);
		}
	}

	auto render(const Memory& mem) -> bool {

		auto i = mem.read(0x0000);
		i++;

		// bgb palette
		uint8_t colors[4][3] = {
			{0xe0, 0xf8, 0xd0},
			{0x34, 0x68, 0x56},
			{0x88, 0xc0, 0x70},
			{0x08, 0x18, 0x20},
		};

		SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
		SDL_RenderClear(renderer_);
		SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);

		for (auto y = 0; y < 144; ++y) {
			for (auto x = 0; x < 160; ++x) {
				const auto pixel = display_[x][y];
				SDL_SetRenderDrawColor(renderer_, colors[pixel][0], colors[pixel][1], colors[pixel][2], 255);
				SDL_RenderDrawPoint(renderer_, x, y);
			}
		}

		SDL_RenderPresent(renderer_);
		SDL_Event e;

		while(SDL_PollEvent(&e) != 0 ) {
			//User requests quit
			if(e.type == SDL_QUIT ) {
				return false;
			}
		}
		return true;
	}

private:
	auto update_map(const Memory& mem) -> void {
		auto counter = 0;
		const auto tile_map = (((mem.read(0xff40) >> 3) & 1) == 1) ? 0x9c00 : 0x9800;
		for (auto ty = 0; ty < 32; ++ty) {
			for (auto tx = 0; tx < 32; ++tx) {

				const auto index = tile_map + (ty) * 32 + tx;
				const auto tile_id = mem.read(index);
				const auto tile = load_tile(mem, 0x8000 + tile_id * 0x10);

				// std::cout << std::hex;
				// std::cout << "INFO: index " << index << '\n';
				// std::cout << "INFO: " << counter++ << ") tile_id " << (int)tile_id << '\n';
				// std::cout << std::dec;

				for (auto x = 0; x < 8; ++x) {
					for (auto y = 0; y < 8; ++y) {
						const auto& pixel = tile[y * 8 + x];
						buffer_[x + tx * 8][y + ty * 8] = pixel;
					}
				}

			}
		}

		// for (auto y = 0; y < 256; ++y) {
			// for (auto x = 0; x < 256; ++x) {
				// const auto val = buffer_[x][y];
				// if (val == 0)
					// std::cout << ' ';
				// else if (val == 1)
					// std::cout << 'x';
				// else if (val == 2)
					// std::cout << 'o';
				// else if (val == 3)
					// std::cout << '_';
			// }
			// std::cout << '\n';
		// }
		// throw std::exception{};
	}

	auto load_tile(const Memory& mem, const uint16_t& addr) -> std::array<uint8_t, 64> {
		auto result = std::array<uint8_t, 64>{};

		for (auto row = 0; row < 8; ++row) {
			const auto first_byte = mem.read(addr + row * 2 + 0);
			const auto second_byte = mem.read(addr + row * 2 + 1);
			result[8 * row + 0] = ((first_byte & 0x80) >> 6) + ((second_byte & 0x80) >> 7);
			result[8 * row + 1] = ((first_byte & 0x40) >> 5) + ((second_byte & 0x40) >> 6);
			result[8 * row + 2] = ((first_byte & 0x20) >> 4) + ((second_byte & 0x20) >> 5);
			result[8 * row + 3] = ((first_byte & 0x10) >> 3) + ((second_byte & 0x10) >> 4);
			result[8 * row + 4] = ((first_byte & 0x08) >> 2) + ((second_byte & 0x08) >> 3);
			result[8 * row + 5] = ((first_byte & 0x04) >> 1) + ((second_byte & 0x04) >> 2);
			result[8 * row + 6] = ((first_byte & 0x02) >> 0) + ((second_byte & 0x02) >> 1);
			result[8 * row + 7] = ((first_byte & 0x01) << 1) + ((second_byte & 0x01) >> 0);
		}

		return result;
	}


	SDL_Renderer* renderer_ = {};
	SDL_Window* window_ = {};
	uint8_t buffer_[256][256] = {};
	uint8_t display_[160][144] = {};

	uint64_t scanline_cycles_ = {};
};

