#include "RenderSoft.hpp"

#include <chrono>
#include <iostream>

#include <GCore/Window.hpp>

#include "ImageView.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	std::cout << "Hello, World!\n";

	//auto window = new Gadget::Window(800, 600, Gadget::RenderAPI::None);
	//delete window;

	SDL_Window* window = SDL_CreateWindow("Software Rendering Surface", 800, 600, 0);
	SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

	auto imageView = RS::ImageView(screenSurface);
	while (true)
	{
		SDL_Event e{};
		if (SDL_PollEvent(&e) && e.type == SDL_EVENT_QUIT)
		{
			break;
		}

		auto currentTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		imageView.Lock();

		imageView.Clear(Gadget::Vector4(
			std::sin(static_cast<double>(currentTimeMs) / 1000.0),
			std::sin(static_cast<double>(currentTimeMs) / 1000.0 - 1),
			std::sin(static_cast<double>(currentTimeMs) / 1000.0 - 2),
			1.0
		));

		imageView.Unlock();

		SDL_UpdateWindowSurface(window);
	}

	SDL_DestroyWindow(window);
	return 0;
}
