#pragma once

#include <chrono>

class Fps {
public:
	auto next_frame() -> void
	{
		++frames_;

		const auto now = std::chrono::high_resolution_clock::now();
		const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_).count();

		if (diff >= 1000) {
			std::cout << "FPS: " << frames_ / (1000.0 / diff) << '\n';
			start_ = std::chrono::high_resolution_clock::now();
			frames_ = 0;
		}
	}

private:
	uint64_t frames_ = {};

	std::chrono::time_point<std::chrono::high_resolution_clock> start_ = std::chrono::high_resolution_clock::now();
};
