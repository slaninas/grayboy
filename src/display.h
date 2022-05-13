#pragma once
#include "memory.h"

#include <SDL2/SDL.h>

struct SpritePixel {
	uint8_t render_color = {};
	uint8_t raw_color = {};
	bool render_over_bg = {};
};

struct BackgroundPixel {
	uint8_t render_color = {};
	uint8_t raw_color = {};
};

struct WindowPixel {
	bool active = {};
	uint8_t render_color = {};
	uint8_t raw_color = {};
};

struct Sprite {
	uint8_t tile_number = {};
	bool render_priority = {};
	bool y_flip = {};
	bool x_flip = {};
	std::array<uint8_t, 4> colors = {};

	int16_t pos_x = {};
	int16_t pos_y = {};
};

struct ScanlineInfo {
	uint64_t cycles = {};
	bool sprites_updated = {};
	bool tiles_updated = {};
	bool hblank_issued = {};
};

// Dummy Deiplay for headless run (tests)
template<bool headless>
class Display {
public:
	Display() = default;

	auto update([[maybe_unused]] Memory& mem, [[maybe_unused]] const uint16_t& cycles) -> void {}
	auto render() -> bool
	{
		return true;
	}
};

// Useful sources:
// - https://stackoverflow.com/a/35989490/1112468
// - http://emudev.de/gameboy-emulator/%e2%af%88-ppu-rgb-arrays-and-sdl/
// - http://www.codeslinger.co.uk/pages/projects/gameboy.html
template<>
class Display<false> {
public:
	const int32_t WIDTH = 160;
	const int32_t HEIGHT = 144;
	const int32_t PIXEL_SCALE = 4;

	const uint64_t CYCLES_PER_FRAME = 70'224;
	const uint64_t CYCLES_PER_SCANLINE = 456 / 4;

	Display()
	{
		if (!SDL_WasInit(SDL_INIT_VIDEO)) {
			SDL_InitSubSystem(SDL_INIT_VIDEO);
		}

		SDL_Window* window =
		  SDL_CreateWindow("GrayBoy", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH * PIXEL_SCALE, HEIGHT * PIXEL_SCALE, 0);

		window_.reset(window);

		window_surface_ = SDL_GetWindowSurface(window_.get());
		surface_.reset(SDL_CreateRGBSurface(0, WIDTH, HEIGHT, 32, 0, 0, 0, 0));

		frame_start_ = SDL_GetTicks();
	}

	auto update(Memory& mem, const uint16_t& cycles) -> void
	{
		lcd_enabled_ = static_cast<bool>(mem.direct_read(0xff40) & (1 << 7));

		frame_cycles_ += cycles;
		if (lcd_enabled_) {
			if (frame_cycles_ >= CYCLES_PER_FRAME) {
				frame_cycles_ -= CYCLES_PER_FRAME;
			}
		}

		else {
			scanline_info_ = {};
			mem.direct_write(0xff44, 0);
			const auto stat = mem.direct_read(0xff41);
			mem.direct_write(0xff41, stat & (~0x3));

			return;
		}

		scanline_info_.cycles += cycles;

		const auto stat = mem.direct_read(0xff41);
		const auto scanline = mem.direct_read(0xff44);

		auto request_interupt = false;
		const auto orig_status = stat & 0x3;
		auto new_status = 0;

		if (scanline_info_.cycles < 80 / 4) {
			new_status = 2;
			request_interupt = static_cast<bool>(stat & (1 << 5));
			if (!scanline_info_.sprites_updated) {
				update_sprites(mem);
				scanline_info_.sprites_updated = true;
			}
		}
		else if (scanline_info_.cycles <= (80 + 172) / 4) {
			new_status = 3;
			if (!scanline_info_.tiles_updated) {
				update_tiles_scanline(mem);
				update_window(mem);
				scanline_info_.tiles_updated = true;
			}
		}

		else {
			new_status = 0;
			if (!scanline_info_.hblank_issued) {
				request_interupt = static_cast<bool>(stat & (1 << 3));
				scanline_info_.hblank_issued = true;
			}
		}

		if (new_status != orig_status && request_interupt) {
			// Request interupt
			mem.direct_write(0xff0f, mem.direct_read(0xff0f) | 0x2);
		}

		mem.direct_write(0xff41, (stat & ~0x3) | new_status);

		if (scanline_info_.cycles >= CYCLES_PER_SCANLINE) {
			scanline_info_.cycles -= CYCLES_PER_SCANLINE;
			scanline_info_.tiles_updated = scanline_info_.sprites_updated = scanline_info_.hblank_issued = vblank_issued_ = false;

			if (scanline == 0x90 && !vblank_issued_) {
				mem.direct_write(0xff0f, mem.direct_read(0xff0f) | 0x1);
				vblank_issued_ = true;
			}

			if (scanline + 1 >= 0x9a) {
				mem.direct_write(0xff44, 0);
			}
			else {
				mem.direct_write(0xff44, scanline + 1);
			}
			check_lyc(mem);
		}
	}

	auto update_surface() -> void
	{
		// https://www.deviantart.com/thewolfbunny/art/Game-Boy-Palette-Grand-Ivory-881455013
		const auto colors = std::array<std::array<uint8_t, 4>, 4>{
		  {{0xd9, 0xd6, 0xbe, 0xff}, {0xa5, 0xa3, 0x91, 0xff}, {0x66, 0x64, 0x59, 0xff}, {0x26, 0x25, 0x21, 0xff}}};

		const auto sdl_colors = std::array<Uint32, 4>{
		  SDL_MapRGBA(surface_->format, colors[0][0], colors[0][1], colors[0][2], 0xff),
		  SDL_MapRGBA(surface_->format, colors[1][0], colors[1][1], colors[1][2], 0xff),
		  SDL_MapRGBA(surface_->format, colors[2][0], colors[2][1], colors[2][2], 0xff),
		  SDL_MapRGBA(surface_->format, colors[3][0], colors[3][1], colors[3][2], 0xff),
		};

		SDL_LockSurface(surface_.get());

		for (auto y = 0; y < 144; ++y) {
			for (auto x = 0; x < 160; ++x) {
				const auto background_pixel = bg_buffer_[y][x];
				auto pixel = background_pixel.render_color;

				const auto sprite_pixel = sprites_buffer_[y][x];

				if (window_buffer_[y][x].active) {
					pixel = window_buffer_[y][x].render_color;
				}
				else if (sprite_pixel.raw_color != 0) {
					// Sprite is under background
					if (sprite_pixel.render_over_bg) {
						pixel = sprite_pixel.render_color;
					}
					else if (background_pixel.raw_color == 0) {
						pixel = sprite_pixel.render_color;
					}
				}
				auto * target_pixel = (Uint32*)((Uint8*)surface_->pixels + y * surface_->pitch + x * surface_->format->BytesPerPixel);
				*target_pixel = sdl_colors[pixel];
			}
		}

		SDL_UnlockSurface(surface_.get());
	}

	static auto check_lyc(Memory& mem) -> void
	{
		const auto stat = mem.direct_read(0xff41);
		auto new_status = stat & 0x3;

		// LY == LYC
		if (mem.direct_read(0xff44) == mem.direct_read(0xff45)) {
			new_status |= 1 << 2;
			if (stat & (1 << 6)) {
				// Request interupt
				mem.direct_write(0xff0f, mem.direct_read(0xff0f) | 0x2);
			}
		}
		else {
			new_status &= ~(1 << 2);
		}

		mem.direct_write(0xff41, (stat & ~0x3) | new_status);
	}

	auto render() -> void
	{

		update_surface();
		SDL_BlitScaled(surface_.get(), nullptr, window_surface_, nullptr);
		SDL_UpdateWindowSurface(window_.get());

		// Use 17 ms per frame, this is close to the actuall hardware
		const auto frame_should_take = 17;
		const auto frame_time = SDL_GetTicks() - frame_start_;

		if (frame_time < frame_should_take) {
			SDL_Delay(frame_should_take - frame_time);
		}

		frame_start_ = SDL_GetTicks();
	}

private:
	auto update_tiles_scanline(const Memory& mem) -> void
	{
		const auto scanline = mem.direct_read(0xff44);
		if (scanline >= 0x90) {
			return;
		}

		if (!(mem.direct_read(0xff40) & 0x1)) {
			for (auto x = size_t{0}; x < size_t{160}; ++x) { bg_buffer_[scanline][x] = {uint8_t{0}, uint8_t{0}}; }
			return;
		}

		const auto palette = mem.direct_read(0xff47);
		const auto colors = std::array{palette & 0x3, (palette & 0xc) >> 2, (palette & 0x30) >> 4, (palette & 0xc0) >> 6};

		const auto SCY = mem.direct_read(0xff42);
		const auto SCX = mem.direct_read(0xff43);

		const auto tile_data = ((mem.direct_read(0xff40) >> 4) & 1) ? 0x8000 : 0x8800;
		const auto tile_map = (((mem.direct_read(0xff40) >> 3) & 1) == 1) ? 0x9c00 : 0x9800;

		const auto pos_y = (scanline + SCY) % 256;

		for (auto x = 0; x < 160; ++x) {
			const auto pos_x = (x + SCX) % 256;
			const auto tile_index = tile_map + pos_y / 8 * 32 + pos_x / 8;
			const auto tile_id = mem.direct_read(tile_index);
			const auto tile_address = tile_data == 0x8000
			  ? (0x8000 + tile_id * 0x10)
			  : ((tile_id < 128 ? 0x9000 + tile_id * 0x10 : 0x8800 + (tile_id - 128) * 0x10));

			const auto first_byte = mem.direct_read(tile_address + (pos_y % 8) * 2 + 1);
			const auto second_byte = mem.direct_read(tile_address + (pos_y % 8) * 2 + 0);

			const auto first_bit = static_cast<bool>(first_byte & (1 << (7 - pos_x % 8)));
			const auto second_bit = static_cast<bool>(second_byte & (1 << (7 - pos_x % 8)));

			const auto pixel = (static_cast<uint8_t>(first_bit) << 1) + second_bit;
			bg_buffer_[scanline][x] = {static_cast<uint8_t>(colors[pixel]), static_cast<uint8_t>(pixel)};
		}
	}

	auto update_window(const Memory& mem) -> void
	{
		const auto scanline = mem.direct_read(0xff44);
		if (scanline >= 0x90) {
			return;
		}

		const auto palette = mem.direct_read(0xff47);
		const auto colors = std::array{palette & 0x3, (palette & 0xc) >> 2, (palette & 0x30) >> 4, (palette & 0xc0) >> 6};

		const auto window_y = mem.direct_read(0xff4A);
		const auto window_x = mem.direct_read(0xff4B) - 0x7;

		const auto using_window = (mem.direct_read(0xff40) & (1 << 5)) && window_y <= scanline;
		if (!using_window) {
			for (auto x = size_t{0}; x < 160; ++x) { window_buffer_[scanline][x] = {}; }
			return;
		}

		const auto tile_data = ((mem.direct_read(0xff40) >> 4) & 1) ? 0x8000 : 0x8800;
		const auto tile_map = ((mem.direct_read(0xff40) >> 6 & 1) == 1) ? 0x9c00 : 9800;

		const auto pos_y = scanline - window_y;

		for (auto x = std::max(0, window_x); x < 160; ++x) {
			const auto pos_x = x - window_x;
			const auto tile_index = tile_map + pos_y / 8 * 32 + pos_x / 8;
			const auto tile_id = mem.direct_read(tile_index);
			const auto tile_address = tile_data == 0x8000
			  ? (0x8000 + tile_id * 0x10)
			  : ((tile_id < 128 ? 0x9000 + tile_id * 0x10 : 0x8800 + (tile_id - 128) * 0x10));

			const auto first_byte = mem.direct_read(tile_address + (pos_y % 8) * 2 + 1);
			const auto second_byte = mem.direct_read(tile_address + (pos_y % 8) * 2 + 0);

			const auto first_bit = static_cast<bool>(first_byte & (1 << (7 - pos_x % 8)));
			const auto second_bit = static_cast<bool>(second_byte & (1 << (7 - pos_x % 8)));

			const auto pixel = (static_cast<uint8_t>(first_bit) << 1) + second_bit;
			window_buffer_[scanline][x] = {true, static_cast<uint8_t>(colors[pixel]), static_cast<uint8_t>(pixel)};
		}
	}

	auto update_sprites(const Memory& mem) -> void
	{
		const auto scanline = mem.direct_read(0xff44);
		if (scanline >= 0x90) {
			return;
		}

		for (auto x = size_t{0}; x < 160; ++x) { sprites_buffer_[scanline][x] = {}; }

		// Objects disabled
		if (!(mem.direct_read(0xff40) & (1 << 1))) {
			return;
		}

		const auto sprites = get_filtered_sprites(get_all_sprites(mem), scanline);

		std::for_each(cbegin(sprites), cend(sprites), [&mem, this, scanline](const auto& s) {
			auto tile = load_tile(mem, 0x8000 + s.tile_number * 0x10, s.x_flip, s.y_flip);

			const auto pixel_y = scanline - s.pos_y;
			for (auto x = std::max(int16_t{0}, s.pos_x); x < std::min(s.pos_x + 8, 160); ++x) {
				const auto pixel_x = x - s.pos_x;
				const auto pixel_val = tile[pixel_x + 8 * pixel_y];
				if (pixel_val != 0) {
					sprites_buffer_[scanline][x] = {s.colors[pixel_val], pixel_val, !s.render_priority};
				}
			}
		});
	}

	static auto get_all_sprites(const Memory& mem) -> std::vector<Sprite>
	{
		auto all_sprites = std::vector<Sprite>{};
		all_sprites.reserve(40);

		const auto large_sprites = mem.direct_read(0xff40) & (1 << 2);

		for (auto sprite = 0; sprite < 40; ++sprite) {
			const auto index = sprite * 4;
			const auto x_pos = static_cast<int16_t>(mem.direct_read(0xfe00 + index + 1) - 0x8);
			const auto y_pos = static_cast<int16_t>(mem.direct_read(0xfe00 + index) - 0x10);
			const auto tile_number = mem.direct_read(0xfe00 + index + 2);

			const auto attrs_raw = mem.direct_read(0xfe00 + index + 3);
			const auto palette_address = (attrs_raw & (1 << 4)) ? 0xff49 : 0xff48;
			const auto palette = mem.direct_read(palette_address);

			const auto s = Sprite{
			  .tile_number = tile_number,
			  .render_priority = static_cast<bool>(attrs_raw & (1 << 7)),
			  .y_flip = static_cast<bool>(attrs_raw & (1 << 6)),
			  .x_flip = static_cast<bool>(attrs_raw & (1 << 5)),
			  .colors =
			    {
			      static_cast<uint8_t>(palette & 0x3),
			      static_cast<uint8_t>((palette & 0xc) >> 2),
			      static_cast<uint8_t>((palette & 0x30) >> 4),
			      static_cast<uint8_t>((palette & 0xc0) >> 6),
			    },
			  .pos_x = x_pos,
			  .pos_y = y_pos,

			};
			all_sprites.push_back(s);
			if (large_sprites) {
				auto sprite2 = s;
				++sprite2.tile_number;
				sprite2.pos_y += 8;
				all_sprites.push_back(sprite2);
			}
		}

		return all_sprites;
	}

	[[nodiscard]] static auto get_filtered_sprites(std::vector<Sprite> sprites, const uint8_t& scanline) -> std::vector<Sprite>
	{
		sprites.erase(
		  std::remove_if(
		    begin(sprites),
		    end(sprites),
		    [scanline](const auto& s) {
			    return !(s.pos_x + 7 >= 0 && s.pos_x < 160 && s.pos_y + 7 >= scanline && s.pos_y <= scanline);
		    }),
		  end(sprites));

		// Get first 10 based on lowest x, then sort them in reverse
		std::stable_sort(begin(sprites), end(sprites), [](const auto& a, const auto& b) { return a.pos_x < b.pos_x; });

		const auto last = sprites.size() >= 10 ? cbegin(sprites) + 10 : cend(sprites);
		sprites.erase(last, cend(sprites));
		std::stable_sort(begin(sprites), end(sprites), [](const auto& a, const auto& b) { return a.pos_x >= b.pos_x; });

		return sprites;
	}

	static auto load_tile(const Memory& mem, const uint16_t& addr, const bool& x_flip, const bool& y_flip) -> std::array<uint8_t, 64>
	{
		auto tile = std::array<uint8_t, 64>{};

		for (auto row = 0; row < 8; ++row) {
			const auto first_byte = mem.direct_read(addr + row * 2 + 1);
			const auto second_byte = mem.direct_read(addr + row * 2 + 0);
			tile[8 * row + 0] = ((first_byte & 0x80) >> 6) + ((second_byte & 0x80) >> 7);
			tile[8 * row + 1] = ((first_byte & 0x40) >> 5) + ((second_byte & 0x40) >> 6);
			tile[8 * row + 2] = ((first_byte & 0x20) >> 4) + ((second_byte & 0x20) >> 5);
			tile[8 * row + 3] = ((first_byte & 0x10) >> 3) + ((second_byte & 0x10) >> 4);
			tile[8 * row + 4] = ((first_byte & 0x08) >> 2) + ((second_byte & 0x08) >> 3);
			tile[8 * row + 5] = ((first_byte & 0x04) >> 1) + ((second_byte & 0x04) >> 2);
			tile[8 * row + 6] = ((first_byte & 0x02) >> 0) + ((second_byte & 0x02) >> 1);
			tile[8 * row + 7] = ((first_byte & 0x01) << 1) + ((second_byte & 0x01) >> 0);
		}

		if (x_flip) {
			for (auto y = size_t{0}; y < 8; ++y) {
				for (auto x = size_t{0}; x < 4; ++x) { std::swap(tile[x + 8 * y], tile[7 - x + 8 * y]); }
			}
		}

		if (y_flip) {
			for (auto y = size_t{0}; y < 4; ++y) {
				for (auto x = size_t{0}; x < 8; ++x) { std::swap(tile[x + 8 * y], tile[x + 8 * (7 - y)]); }
			}
		}

		return tile;
	}

	std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window_ = {nullptr, SDL_DestroyWindow};
	// This is part of the window and it's destroyed with the window, do not free it manually
	SDL_Surface * window_surface_ = {};
	std::unique_ptr<SDL_Surface, decltype(&SDL_FreeSurface)> surface_ = {nullptr, SDL_FreeSurface};

	std::array<std::array<WindowPixel, 160>, 144> window_buffer_;
	std::array<std::array<BackgroundPixel, 160>, 144> bg_buffer_;
	std::array<std::array<SpritePixel, 160>, 144> sprites_buffer_;

	uint64_t frame_cycles_ = {};
	Uint32 frame_start_ = {};
	bool lcd_enabled_ = true;
	bool vblank_issued_ = {};

	ScanlineInfo scanline_info_ = {};
};

/*
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
*/
