#include "RenderSoft.hpp"

#include <chrono>
#include <iostream>

#include <GCore/Window.hpp>

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	std::cout << "Hello, World!\n";

	//auto window = new Gadget::Window(800, 600, Gadget::RenderAPI::None);
	//delete window;

	SDL_Window* window = SDL_CreateWindow("Software Rendering Surface", 800, 600, 0);
	SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

	while (true)
	{
		SDL_Event e{};
		if (SDL_PollEvent(&e) && e.type == SDL_EVENT_QUIT)
		{
			break;
		}

		auto currentTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		SDL_LockSurface(screenSurface);

		const auto pixels = static_cast<uint32_t*>(screenSurface->pixels);
		const auto width = screenSurface->w;
		const auto height = screenSurface->h;
		const auto pitchInPixels = screenSurface->pitch / sizeof(screenSurface->pitch);
		const auto redColor = SDL_MapSurfaceRGB(screenSurface,
			std::sin(static_cast<double>(currentTimeMs) / 1000.0) * 255,
			std::sin(static_cast<double>(currentTimeMs) / 1000.0 - 1) * 255,
			std::sin(static_cast<double>(currentTimeMs) / 1000.0 - 2) * 255
		);

		for(int y = 0; y < height; y++)
		{
			for(int x = 0; x < width; x++)
			{
				pixels[y * pitchInPixels + x] = redColor;
			}
		}

		SDL_UnlockSurface(screenSurface);
		SDL_UpdateWindowSurface(window);
	}

	SDL_DestroyWindow(window);
	return 0;
}
