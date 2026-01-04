#include "RenderSoft.hpp"

#include <chrono>
#include <print>

#include <GCore/Window.hpp>

#include "FrameCounter.hpp"
#include "ImageView.hpp"
#include "Mesh.hpp"

static inline double Det2D(const Gadget::Vector4 & v0, const Gadget::Vector4 & v1)
{
	return v0.x * v1.y - v0.y * v1.x;
}

static void Draw(RS::ImageView& imageView, const RS::Mesh& mesh)
{
	for (size_t v = 0; v + 2 < mesh.vertices.size(); v += 3)
	{
		const auto& pos0 = mesh.vertices[v].position;
		const auto& pos1 = mesh.vertices[v + 1].position;
		const auto& pos2 = mesh.vertices[v + 2].position;

		auto v0 = Gadget::Vector4(pos0.x, pos0.y, pos0.z);
		auto v1 = Gadget::Vector4(pos1.x, pos1.y, pos1.z);
		auto v2 = Gadget::Vector4(pos2.x, pos2.y, pos2.z);

		for (int y = 0; y < imageView.Height(); y++)
		{
			for (int x = 0; x < imageView.Width(); x++)
			{
				auto p = Gadget::Vector4(x + 0.5f, y + 0.5f, 0.0f, 0.0f);

				auto det01p = Det2D(v1 - v0, p - v0);
				auto det12p = Det2D(v2 - v1, p - v1);
				auto det20p = Det2D(v0 - v2, p - v2);

				if (det01p >= 0.0f && det12p >= 0.0f && det20p >= 0.0f)
				{
					imageView.AssignPixel(x, y, mesh.color);
				}
			}
		}
	}
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	std::println("Hello, World!");

	//auto window = new Gadget::Window(800, 600, Gadget::RenderAPI::None);
	//delete window;

	SDL_Window* window = SDL_CreateWindow("Software Rendering Surface", 800, 600, 0);
	SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

	RS::FrameCounter counter;

	auto prevTime = std::chrono::system_clock::now().time_since_epoch();
	auto curTime = prevTime;

	auto imageView = RS::ImageView(screenSurface);

	auto triMesh = RS::Mesh();
	triMesh.vertices.reserve(3);
	triMesh.vertices.emplace_back(Gadget::Vector3(100, 100, 0.0));
	triMesh.vertices.emplace_back(Gadget::Vector3(200, 100, 0.0));
	triMesh.vertices.emplace_back(Gadget::Vector3(100, 200, 0.0));
	triMesh.color = Gadget::Vector4(1.0, 0.0, 0.0, 1.0);

	while (true)
	{
		SDL_Event e{};
		if (SDL_PollEvent(&e) && e.type == SDL_EVENT_QUIT)
		{
			break;
		}

		curTime = std::chrono::system_clock::now().time_since_epoch();
		counter.AddFrameTime(std::chrono::duration_cast<std::chrono::microseconds>(curTime - prevTime));

		auto currentTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		imageView.Lock();

		imageView.Clear(Gadget::Vector4(
			std::sin(static_cast<double>(currentTimeMs) / 1000.0),
			std::sin(static_cast<double>(currentTimeMs) / 1000.0 - 1),
			std::sin(static_cast<double>(currentTimeMs) / 1000.0 - 2),
			1.0
		));

		Draw(imageView, triMesh);

		imageView.Unlock();

		SDL_UpdateWindowSurface(window);
		prevTime = curTime;
	}

	SDL_DestroyWindow(window);
	std::println("Average frame time: {:.3f}ms", counter.GetAverageFrameTimeInMicroseconds() / 1'000.0);
	return 0;
}
