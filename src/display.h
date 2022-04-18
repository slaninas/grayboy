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
		 SDL_CreateWindowAndRenderer(160 * 4, 144 * 4, 0, &window_, &renderer_);
		 SDL_RenderSetScale(renderer_, 4.0, 4.0);
	}

	auto update(Memory& mem, const uint16_t& cycles) {
		// TODO: Don't do it every update
		scanline_cycles_ += cycles;

		const auto SCY = mem.read(0xff42);
		const auto SCX = mem.read(0xff43);

		const auto scanline = mem.read(0xff44);

		if (scanline_cycles_ > CYCLES_PER_SCANLINE) {
			scanline_cycles_ -= CYCLES_PER_SCANLINE;

			if (scanline == 0) {
				update_tile_map(mem);
				update_sprites(mem);
			}


			if (scanline >= 0x99) {
				mem.write(0xff44, 0);
				return;
			}

			if (scanline < 0x90) {

				for (auto x = 0; x < 160; ++x) {
					display_[x][scanline] = 3;
				}

				for (auto x = 0; x < 160; ++x) {
					display_[x][scanline] = bg_buffer_[(x + SCX) % 256][(scanline + SCY) % 256];

					const auto sprite_val = sprites_buffer_[x][scanline];
					if (sprite_val != 3) {
						// std::cout << "INFO: writting sprites_buffer_ " << (int)sprites_buffer_[x][scanline] << '\n';
						if (sprite_val & 0x4) {
							display_[x][scanline] = sprites_buffer_[x][scanline] - 0x4;
						} else {
							if (display_[x][scanline] == 3) {
								display_[x][scanline] = sprites_buffer_[x][scanline];
							}

						}
					}
				}
			}

			// V-BLANK interupt
			if (scanline == 0x90) {
				mem.write(0xff0f, mem.read(0xff0f) | 0x1);
			}
			// TOOD: Write sprites to display_ buffer

			mem.write(0xff44, scanline + 1);
		}
	}

	auto render(Memory& mem) -> bool {

		auto i = mem.read(0x0000);
		i++;

		// bgb palette
		uint8_t colors[4][4] = {
			{0xe0, 0xf8, 0xd0, 0xff},
			{0x88, 0xc0, 0x70, 0xff},
			{0x34, 0x68, 0x56, 0xff},
			{0x08, 0x18, 0x20, 0x00},
			// {0xff, 0x00, 0x00, 0xff},
			// {0x00, 0xff, 0x00, 0xff},
			// {0x00, 0x00, 0xff, 0xff},
			// {0x00, 0x00, 0x00, 0x00},
		};

		SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
		SDL_RenderClear(renderer_);
		SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);

		for (auto y = 0; y < 144; ++y) {
			for (auto x = 0; x < 160; ++x) {
				const auto pixel = display_[x][y];
				SDL_SetRenderDrawColor(renderer_, colors[pixel][0], colors[pixel][1], colors[pixel][2], colors[pixel][3]);
				SDL_RenderDrawPoint(renderer_, x, y);
			}
		}

		SDL_RenderPresent(renderer_);
		SDL_Event event;

		while(SDL_PollEvent(&event) != 0 ) {
			//User requests quit
			//event.type == SDL_KEYDOWN
			if (event.type == SDL_QUIT) {
					return false;
			} else if (event.type == SDL_KEYDOWN) {
				switch(event.key.keysym.sym) {
					case SDLK_RIGHT:
						pressed_key(mem, 0);
						break;
					case SDLK_LEFT:
						pressed_key(mem, 1);
						break;
					case SDLK_UP:
						pressed_key(mem, 2);
						break;
					case SDLK_DOWN:
						pressed_key(mem, 3);
						break;
					case SDLK_a:
						pressed_key(mem, 4);
						break;
					case SDLK_s:
						pressed_key(mem, 5);
						break;
					case SDLK_SPACE:
						pressed_key(mem, 6);
						break;
					case SDLK_RETURN:
						pressed_key(mem, 7);
						break;
				}

			} else if (event.type == SDL_KEYUP) {
				switch(event.key.keysym.sym) {
					case SDLK_RIGHT:
						released_key(mem, 0);
						break;
					case SDLK_LEFT:
						released_key(mem, 1);
						break;
					case SDLK_UP:
						released_key(mem, 2);
						break;
					case SDLK_DOWN:
						released_key(mem, 3);
						break;
					case SDLK_a:
						released_key(mem, 4);
						break;
					case SDLK_s:
						released_key(mem, 5);
						break;
					case SDLK_SPACE:
						released_key(mem, 6);
						break;
					case SDLK_RETURN:
						released_key(mem, 7);
						break;
				}

			}

		}
		return true;
	}

private:

	// see http://www.codeslinger.co.uk/pages/projects/gameboy/joypad.html
	auto pressed_key(Memory& mem, const int& key) -> void {
		auto was_unsed = false;

		if ((mem.joypad_state_ & (1 << key)) == false) {
			was_unsed = true;
		}

		mem.joypad_state_ = (mem.joypad_state_ ^ (1 << key));
		const auto action_button_pressed = key > 3 ? true : false;

		auto request_interupt = false;

		if (!(mem.direct_read(0xff00) & (1 << 5)) && action_button_pressed) {
			request_interupt = true;
		}
		if (!(mem.direct_read(0xff00) & (1 << 4)) && !action_button_pressed) {
			request_interupt = true;
		}

		if (was_unsed && request_interupt) {
			mem.write(0xff0f, mem.read(0xff0f) | 0x16);
		}

	}

	auto released_key(Memory& mem, const uint8_t& key) -> void {
		mem.joypad_state_ ^= 1 << key;
	}

	auto update_tile_map(const Memory& mem) -> void {

		const auto palette = mem.read(0xff47);
		const auto colors = std::array{palette & 0x3, (palette & 0xc) >> 2, (palette & 0x30) >> 4, (palette & 0xc0) >> 6};

		const auto tile_map = (((mem.read(0xff40) >> 3) & 1) == 1) ? 0x9c00 : 0x9800;
		for (auto ty = 0; ty < 32; ++ty) {
			for (auto tx = 0; tx < 32; ++tx) {

				const auto index = tile_map + (ty) * 32 + tx;
				const auto tile_id = mem.read(index);
				const auto tile = load_tile(mem, 0x8000 + tile_id * 0x10);

				for (auto x = 0; x < 8; ++x) {
					for (auto y = 0; y < 8; ++y) {
						const auto& pixel = tile[y * 8 + x];
						// std::cout << "INFO: map, putting " << (int)colors[pixel] << " at the place of " << (int)pixel << '\n';
						bg_buffer_[x + tx * 8][y + ty * 8] = colors[pixel];
					}
				}

			}
		}

	}

	auto update_sprites(const Memory& mem) -> void {

		// TODO: Use flips, background/sprite priority, large sprites etc.
		// const auto large_sprites = mem.read(0xff40) & 0x4;
		raw_dump(mem.dump(), "update_sprites_mem");

		for (auto y = 0; y < 144; ++y) {
			for (auto x = 0; x < 160; ++x) {
				sprites_buffer_[x][y] = 3;
			}

		}

		for (auto sprite = 0; sprite < 40; ++sprite) {
			const auto index = sprite * 4;
			const auto x_pos = mem.read(0xfe00 + index + 1) - 0x8;
			const auto y_pos = mem.read(0xfe00 + index) - 0x10;
			const auto tile_number = mem.read(0xfe00 + index + 2);

			// TODO: Proper selection
			const auto tile_address = 0x8000 + tile_number * 0x10;

			const auto attributes = mem.read(0xfe00 + index + 3);
			const auto render_priority = static_cast<bool>(attributes & (1 << 7));
			const auto y_flip = static_cast<bool>(attributes & (1 << 6));
			const auto x_flip = static_cast<bool>(attributes & (1 << 5));
			// const auto x_flip = false;
			// const auto y_flip = false;
			const auto palette_address = (attributes & (1 << 4)) ? 0xff49 : 0xff48;


			const auto palette = mem.read(palette_address);
			const auto colors = std::array{palette & 0x3, (palette & 0xc) >> 2, (palette & 0x30) >> 4, (palette & 0xc0) >> 6};

			auto tile = load_tile(mem, tile_address);

			if (y_flip) {
				for (auto y = 0; y < 4; ++y) {
					for (auto x = 0; x < 8; ++x) {
						std::swap(tile[y * 8 + x], tile[(7-y)*8 + x]);
					}
				}
			}
			if (x_flip) {
				for (auto y = 0; y < 8; ++y) {
					for (auto x = 0; x < 4; ++x) {
						std::swap(tile[y * 8 + x], tile[y * 8 + (7 - x)]);
					}
				}
			}


			if (x_pos >= 0 && x_pos < 160 && y_pos >= 0 && y_pos < 144) {

				// TODO: FIx case when top left corner of the sprite is out of display but part is
				for (auto y = y_pos; y < std::min(y_pos + 8, 140); ++y) {
					for (auto x = x_pos; x < std::min(x_pos + 8, 160); ++x) {
						const auto value = tile[(y - y_pos) * 8 + x - x_pos];
						// Sprite data 00 is transparent (https://gbdev.gg8.se/wiki/articles/Video_Display#LCD_Monochrome_Palettes)
						if (value != 0) {
							sprites_buffer_[x][y] = colors[value];
							if (!render_priority) {
								sprites_buffer_[x][y] += 0x4;
							}
						}
					}

				}

			}

		}

	}

	auto load_tile(const Memory& mem, const uint16_t& addr) -> std::array<uint8_t, 64> {
		auto result = std::array<uint8_t, 64>{};

		for (auto row = 0; row < 8; ++row) {
			const auto first_byte = mem.read(addr + row * 2 + 1);
			const auto second_byte = mem.read(addr + row * 2 + 0);
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
	uint8_t bg_buffer_[256][256] = {};
	uint8_t sprites_buffer_[160][144] = {};
	uint8_t display_[160][144] = {};

	uint64_t scanline_cycles_ = {};
};

