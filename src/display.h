#include <SDL2/SDL.h>
#include "memory.h"


//https://stackoverflow.com/a/35989490/1112468
class Display {
public:

	Display() {
		 SDL_Init(SDL_INIT_VIDEO);
		 SDL_CreateWindowAndRenderer(160*5, 144 * 5, 0, &window_, &renderer_);
		 SDL_RenderSetScale(renderer_, 5.0, 5.0);
	}

	auto update(const Memory& mem) -> bool {

		uint8_t colors[4][3] = {
			{0, 0, 0},
			{77, 77, 77},
			{155, 155, 155},
			{255, 255, 255},
		};
		while (1) {
			SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
			SDL_RenderClear(renderer_);
			SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);

			for (auto ty = 0; ty < 16; ++ ty) {
				for (auto tx = 0; tx < 16; ++ tx) {
					const auto tile = load_tile(mem, 0x8000 + tx * 0x10 + ty * 0x100);

					for (auto x = 0; x < 8; ++x) {
						for (auto y = 0; y < 8; ++y) {
							const auto& pixel = tile[y * 8 + x];
							SDL_SetRenderDrawColor(renderer_, colors[pixel][0], colors[pixel][1], colors[pixel][2], 255);
							SDL_RenderDrawPoint(renderer_, x + tx * 8, y + ty * 8);
						}
					}
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
			// return false;
		}
		return true;
	}

private:

	auto load_tile(const Memory& mem, const uint16_t& addr) -> std::array<uint8_t, 64> {
		std::cout << std::hex;
		auto result = std::array<uint8_t, 64>{};

		for (auto row = 0; row < 8; ++row) {
			const auto first_byte = mem.read(addr + row * 2 + 0);
			const auto second_byte = mem.read(addr + row * 2 + 1);
			std::cout << "INFO: first_byte 0x" << (int)first_byte << '\n';
			std::cout << "INFO: second_byte 0x" << (int)second_byte << '\n';
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
				std::cout << (int)result[row * 8 + column] << ' ';
			}
			std::cout << '\n';
		}

		std::cout << std::dec;
		return result;
	}


	SDL_Renderer* renderer_ = {};
	SDL_Window* window_ = {};
};

