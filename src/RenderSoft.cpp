#include "RenderSoft.hpp"

#include <chrono>
#include <print>

#include <GCore/Window.hpp>

#include "DrawCall.hpp"
#include "FrameCounter.hpp"
#include "ImageView.hpp"
#include "Mesh.hpp"
#include "MeshAssets.hpp"
#include "Viewport.hpp"

static inline double Det2D(const Gadget::Vector2& v0, const Gadget::Vector2& v1)
{
	return (v0.x * v1.y) - (v0.y * v1.x);
}

static void Draw(const RS::Viewport& viewport, RS::ImageView& imageView, const RS::DrawCall& drawCall)
{
	for (size_t i = 0; i + 2 < drawCall.mesh.indices.size(); i += 3)
	{
		const auto i0 = drawCall.mesh.indices[i];
		const auto i1 = drawCall.mesh.indices[i + 1];
		const auto i2 = drawCall.mesh.indices[i + 2];

		auto clip0 = drawCall.transform * Gadget::Math::ToVector4(drawCall.mesh.vertices[i0].position);
		auto clip1 = drawCall.transform * Gadget::Math::ToVector4(drawCall.mesh.vertices[i1].position);
		auto clip2 = drawCall.transform * Gadget::Math::ToVector4(drawCall.mesh.vertices[i2].position);

		clip0 /= clip0.w;
		clip1 /= clip1.w;
		clip2 /= clip2.w;

		auto v0 = viewport.NdcToViewport(clip0);
		auto v1 = viewport.NdcToViewport(clip1);
		auto v2 = viewport.NdcToViewport(clip2);

		auto c0 = drawCall.mesh.vertices[i0].color;
		auto c1 = drawCall.mesh.vertices[i1].color;
		auto c2 = drawCall.mesh.vertices[i2].color;

		auto det012 = Det2D(v1 - v0, v2 - v0);
		const bool ccw = det012 < 0.0;
		if (ccw && drawCall.mode == RS::CullMode::CW || !ccw && drawCall.mode == RS::CullMode::CCW)
		{
			continue; // Skip this triangle (back-face culling)
		}

		if (ccw)
		{
			std::swap(v1, v2);
			det012 = -det012;
		}

		std::array<Gadget::Vector2, 3> verts = { v0, v1, v2 };
		auto bounds = Gadget::Math::CalculateBounds<double>(verts); // TODO - span was a nice idea, but the dev UX here kinda blows

		if (bounds.min.x >= imageView.Width() || bounds.min.y >= imageView.Height() || bounds.max.x < 0.0 || bounds.max.y < 0.0)
		{
			continue; // Early out, triangle bounds are fully off-screen
		}

		// Ignore any part of the bounds that are off-screen
		bounds.min.x = std::max<double>(bounds.min.x, viewport.GetXMin());
		bounds.min.y = std::max<double>(bounds.min.y, viewport.GetYMin());
		bounds.max.x = std::min<double>(std::min<double>(bounds.max.x, imageView.Width() - 1), viewport.GetXMax());
		bounds.max.y = std::min<double>(std::min<double>(bounds.max.y, imageView.Height() - 1), viewport.GetYMax());

		for (int y = bounds.min.y; y < bounds.max.y; y++)
		{
			for (int x = bounds.min.x; x < bounds.max.x; x++)
			{
				auto p = Gadget::Vector2(static_cast<double>(x) + 0.5, static_cast<double>(y) + 0.5);

				auto det01p = Det2D(v1 - v0, p - v0);
				auto det12p = Det2D(v2 - v1, p - v1);
				auto det20p = Det2D(v0 - v2, p - v2);

				if (det01p >= 0.0f && det12p >= 0.0f && det20p >= 0.0f)
				{
					const auto l0 = det12p / det012;
					const auto l1 = det20p / det012;
					const auto l2 = det01p / det012;

					auto finalColor = (c0 * l0) + (c1 * l1) + (c2 * l2);
					imageView.AssignPixel(x, y, finalColor);
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

	SDL_Window* window = SDL_CreateWindow("Software RasterMan", screenW, screenH, 0);
	SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

	RS::FrameCounter counter;

	auto prevTime = std::chrono::system_clock::now().time_since_epoch();
	auto curTime = prevTime;

	auto imageView = RS::ImageView(screenSurface);

	auto viewport = RS::Viewport(0, screenW, 0, screenH);

	//auto rectMesh = RS::GetRectMesh();
	auto cubeMesh = RS::GetCubeMesh();

	auto aspect = screenW * 1.0 / screenH;

	auto transform = Gadget::Matrix4::Identity();
	transform *= Gadget::Math::Scale(Gadget::Vector3(0.0025, 0.0025, 0.0025));
	transform *= Gadget::Math::Translate(Gadget::Vector3(0.0, 0.0, -100.0));
	transform *= Gadget::Matrix4::Perspective(Gadget::Math::Pi / 3.0, aspect, 0.01, 100.0);

	while (true)
	{
		SDL_Event e{};
		if (SDL_PollEvent(&e) && e.type == SDL_EVENT_QUIT)
		{
			break;
		}

		curTime = std::chrono::system_clock::now().time_since_epoch();
		counter.AddFrameTime(std::chrono::duration_cast<std::chrono::microseconds>(curTime - prevTime));

		const auto deltaTime = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - prevTime).count()) / 1000.0;
		transform *= Gadget::Math::Rotate(deltaTime * 25.0, Gadget::Vector3(0.0, 1.0, 0.0));
		transform *= Gadget::Math::Rotate(deltaTime * 25.0 * 1.5, Gadget::Vector3(1.0, 0.0, 0.0));

		imageView.Lock();
		imageView.Clear(Gadget::Color(0.1, 0.1, 0.1));

		Draw(viewport, imageView, RS::DrawCall(cubeMesh, transform));

		imageView.Unlock();

		SDL_UpdateWindowSurface(window);
		prevTime = curTime;
	}

	SDL_DestroyWindow(window);
	std::println("Average frame time: {:.3f}ms", counter.GetAverageFrameTimeInMicroseconds() / 1'000.0);
	return 0;
}
