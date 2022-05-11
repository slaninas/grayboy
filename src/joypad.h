#pragma once

#include <SDL2/SDL.h>

struct JoypadUpdate {
	bool quit = {};
	bool request_interupt = {};
	uint8_t state = {};
};

class Joypad {
public:
	Joypad() = default;

	auto update(const uint8_t& joyp) -> JoypadUpdate
	{
		SDL_Event event;

		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) { return {.quit = true}; }
		}

		auto request_interupt = false;
		const auto keys_state = SDL_GetKeyboardState(nullptr);

		if (keys_state[SDL_SCANCODE_RIGHT]) { request_interupt |= pressed_key(joyp, 0); }
		else {
			released_key(0);
		}

		if (keys_state[SDL_SCANCODE_LEFT]) { request_interupt |= pressed_key(joyp, 1); }
		else {
			released_key(1);
		}

		if (keys_state[SDL_SCANCODE_UP]) { request_interupt |= pressed_key(joyp, 2); }
		else {
			released_key(2);
		}

		if (keys_state[SDL_SCANCODE_DOWN]) { request_interupt |= pressed_key(joyp, 3); }
		else {
			released_key(3);
		}

		if (keys_state[SDL_SCANCODE_A]) { request_interupt |= pressed_key(joyp, 4); }
		else {
			released_key(4);
		}

		if (keys_state[SDL_SCANCODE_S]) { request_interupt |= pressed_key(joyp, 5); }
		else {
			released_key(5);
		}

		if (keys_state[SDL_SCANCODE_SPACE]) { request_interupt |= pressed_key(joyp, 6); }
		else {
			released_key(6);
		}

		if (keys_state[SDL_SCANCODE_RETURN]) { request_interupt |= pressed_key(joyp, 7); }
		else {
			released_key(7);
		}

		return {.quit = false, .request_interupt = request_interupt, .state = joypad_state_};
	}

private:
	// see http://www.codeslinger.co.uk/pages/projects/gameboy/joypad.html
	auto pressed_key(const uint8_t& joyp, const int& key) -> bool
	{
		auto was_unsed = false;

		if ((joypad_state_ & (1 << key)) == false) { was_unsed = true; }

		joypad_state_ = (joypad_state_ | (1 << key));
		const auto action_button_pressed = key > 3 ? true : false;

		auto request_interupt = false;

		if (!(joyp & (1 << 5)) && action_button_pressed) { request_interupt = true; }
		if (!(joyp & (1 << 4)) && !action_button_pressed) { request_interupt = true; }

		return was_unsed && request_interupt;
	}

	auto released_key(const uint8_t& key) -> void
	{
		joypad_state_ = joypad_state_ & ~(1 << key);
	}

	uint8_t joypad_state_ = {};
};
