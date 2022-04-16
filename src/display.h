#pragma once
#include <SDL2/SDL.h>
#include "memory.h"


// Useful sources:
// - https://stackoverflow.com/a/35989490/1112468
// - http://emudev.de/gameboy-emulator/%e2%af%88-ppu-rgb-arrays-and-sdl/
class Display {
public:

	Display() {
		 SDL_Init(SDL_INIT_VIDEO);
		 SDL_CreateWindowAndRenderer(160 * 5, 144 * 5, 0, &window_, &renderer_);
		 SDL_RenderSetScale(renderer_, 5.0, 5.0);
	}

	auto update(const Memory& mem) -> bool {

		uint8_t colors[4][3] = {
			{0, 0, 0},
			{77, 77, 77},
			{155, 155, 155},
			{255, 255, 255},
		};
		const auto SCY = mem.read(0xff42);
		const auto SCX = mem.read(0xff43);

		const auto tile_map = (((mem.read(0xff40) >> 3) & 1) == 1) ? 0x9c00 : 0x9800;

		SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
		SDL_RenderClear(renderer_);
		SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);

		uint8_t video[256][256] = {};

		for (auto ty = 0; ty < 32; ++ty) {
			for (auto tx = 0; tx < 32; ++tx) {

				// std::cout << std::hex << "SCY " << (int)SCY << '\n';
				const auto index = tile_map + (ty) * 32 + tx;
				const auto tile_id = mem.read(index);
				const auto tile = load_tile(mem, 0x8000 + tile_id * 0x10);

				for (auto x = 0; x < 8; ++x) {
					for (auto y = 0; y < 8; ++y) {
						const auto& pixel = tile[y * 8 + x];
						video[x + tx * 8][y + ty * 8] = pixel;
					}
				}

			}
		}


		for (auto y = 0; y < 256; ++y) {
			for (auto x = 0; x < 256; ++x) {
				const auto pixel = video[x][(y + SCY) % 256];
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

	auto load_tile(const Memory& mem, const uint16_t& addr) -> std::array<uint8_t, 64> {
		auto result = std::array<uint8_t, 64>{};

		for (auto row = 0; row < 8; ++row) {
			const auto first_byte = mem.read(addr + row * 2 + 0);
			const auto second_byte = mem.read(addr + row * 2 + 1);
			result[8 * row + 0] = ((first_byte & 0x80) >> 7) + ((second_byte & 0x80) >> 7);
			result[8 * row + 1] = ((first_byte & 0x40) >> 6) + ((second_byte & 0x40) >> 6);
			result[8 * row + 2] = ((first_byte & 0x20) >> 5) + ((second_byte & 0x20) >> 5);
			result[8 * row + 3] = ((first_byte & 0x10) >> 4) + ((second_byte & 0x10) >> 4);
			result[8 * row + 4] = ((first_byte & 0x08) >> 3) + ((second_byte & 0x08) >> 3);
			result[8 * row + 5] = ((first_byte & 0x04) >> 2) + ((second_byte & 0x04) >> 2);
			result[8 * row + 6] = ((first_byte & 0x02) >> 1) + ((second_byte & 0x02) >> 1);
			result[8 * row + 7] = ((first_byte & 0x01) >> 0) + ((second_byte & 0x01) >> 0);
		}

			for (auto row = 0; row < 8; ++row) {
				for (auto column = 0; column < 8; ++column) {
			}
		}

		return result;
	}


	SDL_Renderer* renderer_ = {};
	SDL_Window* window_ = {};
};

