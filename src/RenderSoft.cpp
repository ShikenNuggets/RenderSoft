#include "RenderSoft.hpp"

#include <chrono>
#include <print>

#include <GCore/Window.hpp>

#include "FrameCounter.hpp"
#include "ImageView.hpp"
#include "Mesh.hpp"

static inline double Det2D(const Gadget::Vector3& v0, const Gadget::Vector3& v1)
{
	return v0.x * v1.y - v0.y * v1.x;
}

static void Draw(RS::ImageView& imageView, const RS::Mesh& mesh)
{
	for (size_t v = 0; v + 2 < mesh.vertices.size(); v += 3)
	{
		auto v0 = mesh.vertices[v].position;
		auto v1 = mesh.vertices[v + 1].position;
		auto v2 = mesh.vertices[v + 2].position;

		auto det012 = Det2D(v1 - v0, v2 - v0);
		const bool ccw = det012 < 0.0;
		if (det012 < 0.0)
		{
			std::swap(v1, v2);
			det012 = -det012;
		}

		std::array<Gadget::Vector2, 3> verts = {
			Gadget::Vector2(v0.x, v0.y),
			Gadget::Vector2(v1.x, v1.y),
			Gadget::Vector2(v2.x, v2.y)
		};

		const auto bounds = Gadget::Math::CalculateBounds<double>(verts); // TODO - span was a nice idea, but the dev UX here kinda blows

		for (int y = bounds.min.y; y < bounds.max.y; y++)
		{
			for (int x = bounds.min.x; x < bounds.max.x; x++)
			{
				auto p = Gadget::Vector3(x + 0.5f, y + 0.5f, 0.0f);

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
	static constexpr auto screenW = 800;
	static constexpr auto screenH = 600;

	std::println("Hello, World!");

	//auto window = new Gadget::Window(800, 600, Gadget::RenderAPI::None);
	//delete window;

	SDL_Window* window = SDL_CreateWindow("Software Rendering Surface", screenW, screenH, 0);
	SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

	RS::FrameCounter counter;

	auto prevTime = std::chrono::system_clock::now().time_since_epoch();
	auto curTime = prevTime;

	auto imageView = RS::ImageView(screenSurface);

	auto triMesh = RS::Mesh();
	triMesh.vertices.reserve(3);
	triMesh.vertices.emplace_back(Gadget::Vector3(400.0, 100.0, 0.0));
	triMesh.vertices.emplace_back(Gadget::Vector3(600.0, 500.0, 0.0));
	triMesh.vertices.emplace_back(Gadget::Vector3(200.0, 500.0, 0.0));
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

		//auto currentTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		imageView.Lock();

		/*imageView.Clear(Gadget::Vector4(
			std::sin(static_cast<double>(currentTimeMs) / 1000.0),
			std::sin(static_cast<double>(currentTimeMs) / 1000.0 - 1),
			std::sin(static_cast<double>(currentTimeMs) / 1000.0 - 2),
			1.0
		));*/

		imageView.Clear(Gadget::Vector4(0.1, 0.1, 0.1, 1.0));

		Draw(imageView, triMesh);

		imageView.Unlock();

		SDL_UpdateWindowSurface(window);
		prevTime = curTime;
	}

	SDL_DestroyWindow(window);
	std::println("Average frame time: {:.3f}ms", counter.GetAverageFrameTimeInMicroseconds() / 1'000.0);
	return 0;
}
