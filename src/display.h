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
		 // SDL_CreateWindowAndRenderer((160 + 256) * 4, 144 * 4, 0, &window_, &renderer_);
		 SDL_CreateWindowAndRenderer(160 * 4, 144 * 4, 0, &window_, &renderer_);
		 SDL_RenderSetScale(renderer_, 4.0, 4.0);
	}

	auto update(Memory& mem, const uint16_t& cycles) {
		// TODO: Don't do it every update
		scanline_cycles_ += cycles;

		const auto SCY = mem.read(0xff42);
		const auto SCX = mem.read(0xff43);

		const auto scanline = mem.read(0xff44);

		if (scanline_cycles_ >= CYCLES_PER_SCANLINE) {
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
					display_[x][scanline] = 0;
				}

				for (auto x = 0; x < 160; ++x) {
					display_[x][scanline] = bg_buffer_[(x + SCX) % 256][(scanline + SCY) % 256];

				}
				for (auto x = 0; x < 160; ++x) {
					// TODO: Fix sprites rendering
					const auto render_priority = sprites_buffer_[x][scanline] & 0x4;
					const auto sprite_val = sprites_buffer_[x][scanline] & 0x3;

					if (sprite_val != 0) {
						// Sprite is under background
						if (!render_priority) {
							display_[x][scanline] = sprite_val;
						} else if (display_[x][scanline] == 0) {
							display_[x][scanline] = sprite_val;
						}
					}
				}
			}

			// V-BLANK interupt
			if (scanline == 0x90) {
				mem.write(0xff0f, mem.read(0xff0f) | 0x1);
			}

			mem.write(0xff44, scanline + 1);
		}
	}

	auto render(Memory& mem) -> bool {
		uint8_t palettes[][4][4] = {
			// bgb palette
			{{0xe0, 0xf8, 0xd0, 0xff},
			{0x88, 0xc0, 0x70, 0xff},
			{0x34, 0x68, 0x56, 0xff},
			{0x08, 0x18, 0x20, 0x00}},

			// https://www.deviantart.com/thewolfbunny/art/Game-Boy-Palette-Kingdom-Key-Gold-902493926
			{{0xca, 0xad, 0x32, 0xff},
			{0x9a, 0x84, 0x26, 0xff},
			{0x5f, 0x51, 0x17, 0xff},
			{0x23, 0x1e, 0x08, 0xff}},

			// https://www.deviantart.com/thewolfbunny/art/Game-Boy-Palette-Spotify-Green-889906803
			{{0x1e, 0xd7, 0x60, 0xff},
			{0x16, 0xa4, 0x49, 0xff},
			{0x0e, 0x65, 0x2d, 0xff},
			{0x05, 0x25, 0x10, 0xff}},

			// https://www.deviantart.com/thewolfbunny/art/Game-Boy-Palette-Brilliant-Diamond-Blue-889731643
			{{0x7f, 0xbb, 0xe1, 0xff},
			{0x61, 0x8f, 0xac, 0xff},
			{0x3b, 0x58, 0x69, 0xff},
			{0x16, 0x21, 0x27, 0xff}},

			//https://www.deviantart.com/thewolfbunny/art/Game-Boy-Palette-Super-Mushroom-Vision-889340710
			{{0xf7, 0xce, 0xc3, 0xff},
			{0xcc, 0x9e, 0x22, 0xff},
			{0x92, 0x34, 0x04, 0xff},
			{0x00, 0x00, 0x00, 0xff}},

			//https://www.deviantart.com/thewolfbunny/art/Game-Boy-Palette-School-Idol-Mix-883047563
			{{0xf3, 0x98, 0x00, 0xff},
			{0x00, 0xa0, 0xe9, 0xff},
			{0xa5, 0x46, 0x9b, 0xff},
			{0x31, 0x15, 0x2e, 0xff}},

			// https://www.deviantart.com/thewolfbunny/art/Game-Boy-Palette-Grand-Ivory-881455013
			{{0xd9, 0xd6, 0xbe, 0xff},
			{0xa5, 0xa3, 0x91, 0xff},
			{0x66, 0x64, 0x59, 0xff},
			{0x26, 0x25, 0x21, 0xff}},

			// https://www.deviantart.com/thewolfbunny/art/Game-Boy-Palette-Lime-Midori-810574708
			{{0xe0, 0xeb, 0xaf, 0xff},
			{0xaa, 0xcf, 0x53, 0xff},
			{0x7b, 0x8d, 0x42, 0xff},
			{0x47, 0x59, 0x50, 0xff}},

			// https://www.deviantart.com/thewolfbunny/art/Game-Boy-Palette-Ninja-Turtle-Green-810574661
			{{0x8c, 0xc6, 0x3f, 0xff},
			{0x6b, 0x97, 0x30, 0xff},
			{0x41, 0x5d, 0x1d, 0xff},
			{0x18, 0x22, 0x0b, 0xff}},

			// https://www.deviantart.com/thewolfbunny/art/Game-Boy-Palette-Fruity-Orange-810574227
			{{0xf3, 0xbf, 0x88, 0xff},
			{0xf0, 0x83, 0x00, 0xff},
			{0x9f, 0x56, 0x3a, 0xff},
			{0x23, 0x1a, 0x08, 0xff}},

			// Just some grayscale
			{{0xff, 0xff, 0xff, 0xff},
			{0x50, 0x50, 0x50, 0xff},
			{0xa0, 0xa0, 0xa0, 0xff},
			{0x00, 0x00, 0x00, 0x00}},
		};

		const auto colors = palettes[6];

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

		// Render vram
		// const auto tile_colors = palettes[6];
		// for (auto y = 0; y < 16; ++y) {
			// for (auto x = 0; x < 16; ++x) {
				// const auto tile = load_tile(mem, 0x8000 + 0x10 * x + 0x100 * y);

				// for (auto tile_y = 0; tile_y < 8; ++tile_y) {
					// for (auto tile_x = 0; tile_x < 8; ++tile_x) {
						// const auto pixel = tile[tile_x + tile_y * 8];
						// SDL_SetRenderDrawColor(renderer_, tile_colors[pixel][0], tile_colors[pixel][1], tile_colors[pixel][2], tile_colors[pixel][3]);
						// SDL_RenderDrawPoint(renderer_, x * 8 + tile_x + 160, y * 8 + tile_y);
					// }
				// }
			// }

		// }

		SDL_RenderPresent(renderer_);
		SDL_Event event;

		while(SDL_PollEvent(&event) != 0 ) {
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
					// case SDLK_d:
						// render_bg = !render_bg;
						// break;
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
		mem.joypad_state_ = mem.joypad_state_ & ~(1 << key);
	}

	auto update_tile_map(const Memory& mem) -> void {

		const auto palette = mem.read(0xff47);
		const auto colors = std::array{palette & 0x3, (palette & 0xc) >> 2, (palette & 0x30) >> 4, (palette & 0xc0) >> 6};

		const auto tile_map = (((mem.read(0xff40) >> 3) & 1) == 1) ? 0x9c00 : 0x9800;
		const auto tile_data = ((mem.read(0xff40) >> 4) & 1) ? 0x8000 : 0x8800;

		for (auto ty = 0; ty < 32; ++ty) {
			for (auto tx = 0; tx < 32; ++tx) {

				const auto index = tile_map + (ty) * 32 + tx;
				const auto tile_id = mem.read(index);
				const auto tile_address = tile_data == 0x8000 ? (0x8000 + tile_id * 0x10) : ((tile_id < 128 ? 0x9000 + tile_id * 0x10 : 0x8800 + (tile_id - 128) * 0x10));

				const auto tile = load_tile(mem, tile_address);

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

		// TODO: Large sprites
		// const auto large_sprites = mem.read(0xff40) & 0x4;
		raw_dump(mem.dump(), "update_sprites_mem");


		for (auto y = static_cast<uint64_t>(0); y < sprites_buffer_[0].size(); ++y) {
			for (auto x = static_cast<uint64_t>(0); x < sprites_buffer_.size(); ++x) {
				sprites_buffer_[x][y] = 0;
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
							if (render_priority) {
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
	std::array<std::array<uint8_t, 256>, 256> bg_buffer_;
	std::array<std::array<uint8_t, 144>, 160> sprites_buffer_;
	std::array<std::array<uint8_t, 144>, 160> display_;

	uint64_t scanline_cycles_ = {};
};

