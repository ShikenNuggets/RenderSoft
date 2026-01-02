#include "RenderSoft.hpp"

#include <iostream>

#include <GCore/Window.hpp>

int main(int argc, char* argv[])
{
	std::cout << "Hello, World!" << std::endl;

	//auto window = new Gadget::Window(800, 600, Gadget::RenderAPI::None);
	//delete window;

	SDL_Window* window = SDL_CreateWindow("Software Rendering Surface", 800, 600, 0);
	SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

	SDL_LockSurface(screenSurface);

	const auto pixels = static_cast<uint32_t*>(screenSurface->pixels);
	const auto width = screenSurface->w;
	const auto height = screenSurface->h;
	const auto pitchInPixels = screenSurface->pitch / sizeof(screenSurface->pitch);
	const auto redColor = SDL_MapSurfaceRGB(screenSurface, 255, 0, 0);

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			pixels[y * pitchInPixels + x] = redColor;
		}
	}

	SDL_UnlockSurface(screenSurface);
	SDL_UpdateWindowSurface(window);

	std::cin.get();
	SDL_DestroyWindow(window);
	return 0;
}
