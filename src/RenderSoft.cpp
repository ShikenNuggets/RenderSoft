#include "RenderSoft.hpp"

#include <iostream>

#include <GCore/Window.hpp>

int main(int argc, char* argv[])
{
	std::cout << "Hello, World!" << std::endl;

	auto window = new Gadget::Window(800, 600, Gadget::RenderAPI::None);
	int count = 0;
	while (count < 5000000)
	{
		window->HandleEvents();
		count++;
	}

	delete window;
	return 0;
}
