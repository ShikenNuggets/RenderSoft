#include "RenderSoft.hpp"

#include <chrono>
#include <print>

#include <GCore/Window.hpp>

#include "DrawCall.hpp"
#include "FrameCounter.hpp"
#include "ImageView.hpp"
#include "Mesh.hpp"
#include "Viewport.hpp"

static inline double Det2D(const Gadget::Vector2& v0, const Gadget::Vector2& v1)
{
	return v0.x * v1.y - v0.y * v1.x;
}

static void Draw(const RS::Viewport& viewport, RS::ImageView& imageView, const RS::DrawCall& drawCall)
{
	for (size_t i = 0; i + 2 < drawCall.mesh.indices.size(); i += 3)
	{
		const auto i0 = drawCall.mesh.indices[i];
		const auto i1 = drawCall.mesh.indices[i + 1];
		const auto i2 = drawCall.mesh.indices[i + 2];

		const auto v0Pos = drawCall.mesh.vertices[i0].position;
		const auto v1Pos = drawCall.mesh.vertices[i1].position;
		const auto v2Pos = drawCall.mesh.vertices[i2].position;

		auto v0 = viewport.NdcToViewport(drawCall.transform * Gadget::Vector4(v0Pos.x, v0Pos.y, v0Pos.z, 1.0));
		auto v1 = viewport.NdcToViewport(drawCall.transform * Gadget::Vector4(v1Pos.x, v1Pos.y, v1Pos.z, 1.0));
		auto v2 = viewport.NdcToViewport(drawCall.transform * Gadget::Vector4(v2Pos.x, v2Pos.y, v2Pos.z, 1.0));

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
		/*	Gadget::Vector2(v0.x, v0.y),
			Gadget::Vector2(v1.x, v1.y),
			Gadget::Vector2(v2.x, v2.y)
		};*/

		auto bounds = Gadget::Math::CalculateBounds<double>(verts); // TODO - span was a nice idea, but the dev UX here kinda blows

		if (bounds.min.x >= imageView.Width() || bounds.min.y >= imageView.Height() || bounds.max.x < 0.0 || bounds.max.y < 0.0)
		{
			continue; // Early out, triangle bounds are fully off-screen
		}

		// Ignore any part of the bounds that are off-screen
		bounds.min.x = std::max<double>(bounds.min.x, 0.0);
		bounds.min.y = std::max<double>(bounds.min.y, 0.0);
		bounds.max.x = std::min<double>(bounds.max.x, imageView.Width() - 1);
		bounds.max.y = std::min<double>(bounds.max.y, imageView.Height() - 1);

		for (int y = bounds.min.y; y < bounds.max.y; y++)
		{
			for (int x = bounds.min.x; x < bounds.max.x; x++)
			{
				auto p = Gadget::Vector2(x + 0.5f, y + 0.5f);

				auto det01p = Det2D(v1 - v0, p - v0);
				auto det12p = Det2D(v2 - v1, p - v1);
				auto det20p = Det2D(v0 - v2, p - v2);

				if (det01p >= 0.0f && det12p >= 0.0f && det20p >= 0.0f)
				{
					const auto l0 = det12p / det012;
					const auto l1 = det20p / det012;
					const auto l2 = det01p / det012;

					auto colorVec = l0 * c0 + l1 * c1 + l2 * c2;

					imageView.AssignPixel(x, y, Gadget::Color(colorVec.x, colorVec.y, colorVec.z));
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

	SDL_Window* window = SDL_CreateWindow("Software RasterMan", screenW, screenH, 0);
	SDL_Surface* screenSurface = SDL_GetWindowSurface(window);

	RS::FrameCounter counter;

	auto prevTime = std::chrono::system_clock::now().time_since_epoch();
	auto curTime = prevTime;

	auto imageView = RS::ImageView(screenSurface);

	auto viewport = RS::Viewport(0, screenW, 0, screenH);

	auto rectMesh = RS::Mesh();
	rectMesh.vertices.reserve(4);
	rectMesh.vertices.emplace_back(Gadget::Vector3(-0.5, -0.5, 0.0));
	rectMesh.vertices.emplace_back(Gadget::Vector3(-0.5, 0.5, 0.0));
	rectMesh.vertices.emplace_back(Gadget::Vector3(0.5, -0.5, 0.0));
	rectMesh.vertices.emplace_back(Gadget::Vector3(0.5, 0.5, 0.0));

	rectMesh.indices.push_back(0);
	rectMesh.indices.push_back(1);
	rectMesh.indices.push_back(2);

	rectMesh.indices.push_back(2);
	rectMesh.indices.push_back(1);
	rectMesh.indices.push_back(3);

	rectMesh.vertices[0].color = Gadget::Vector4(1.0, 0.0, 0.0, 1.0);
	rectMesh.vertices[1].color = Gadget::Vector4(0.0, 1.0, 0.0, 1.0);
	rectMesh.vertices[2].color = Gadget::Vector4(0.0, 0.0, 1.0, 1.0);
	rectMesh.vertices[3].color = Gadget::Vector4(0.0, 0.0, 0.0, 1.0);


	auto cubeMesh = RS::Mesh();
	cubeMesh.vertices.push_back(Gadget::Vector3(-1.0, -1.0, -1.0));
	cubeMesh.vertices.push_back(Gadget::Vector3(-1.f, 1.f, -1.f));
	cubeMesh.vertices.push_back(Gadget::Vector3(-1.f, -1.f, 1.f));
	cubeMesh.vertices.push_back(Gadget::Vector3(-1.f, 1.f, 1.f));

	// +X face
	cubeMesh.vertices.push_back(Gadget::Vector3(1.f, -1.f, -1.f));
	cubeMesh.vertices.push_back(Gadget::Vector3(1.f, 1.f, -1.f));
	cubeMesh.vertices.push_back(Gadget::Vector3(1.f, -1.f, 1.f));
	cubeMesh.vertices.push_back(Gadget::Vector3(1.f, 1.f, 1.f));

	// -Y face
	cubeMesh.vertices.push_back(Gadget::Vector3(-1.f, -1.f, -1.f));
	cubeMesh.vertices.push_back(Gadget::Vector3(1.f, -1.f, -1.f));
	cubeMesh.vertices.push_back(Gadget::Vector3(-1.f, -1.f, 1.f));
	cubeMesh.vertices.push_back(Gadget::Vector3(1.f, -1.f, 1.f));

	// +Y face
	cubeMesh.vertices.push_back(Gadget::Vector3(-1.f, 1.f, -1.f));
	cubeMesh.vertices.push_back(Gadget::Vector3(1.f, 1.f, -1.f));
	cubeMesh.vertices.push_back(Gadget::Vector3(-1.f, 1.f, 1.f));
	cubeMesh.vertices.push_back(Gadget::Vector3(1.f, 1.f, 1.f));

	// -Z face
	cubeMesh.vertices.push_back(Gadget::Vector3(-1.f, -1.f, -1.f));
	cubeMesh.vertices.push_back(Gadget::Vector3(1.f, -1.f, -1.f));
	cubeMesh.vertices.push_back(Gadget::Vector3(-1.f, 1.f, -1.f));
	cubeMesh.vertices.push_back(Gadget::Vector3(1.f, 1.f, -1.f));

	// +Z face
	cubeMesh.vertices.push_back(Gadget::Vector3(-1.f, -1.f, 1.f));
	cubeMesh.vertices.push_back(Gadget::Vector3(1.f, -1.f, 1.f));
	cubeMesh.vertices.push_back(Gadget::Vector3(-1.f, 1.f, 1.f));
	cubeMesh.vertices.push_back(Gadget::Vector3(1.f, 1.f, 1.f));

	cubeMesh.vertices.push_back(Gadget::Vector3(-1.0, -1.0, -1.0));
	cubeMesh.vertices.push_back(Gadget::Vector3(-1.0, -1.0,  1.0));
	cubeMesh.vertices.push_back(Gadget::Vector3(-1.0,  1.0, -1.0));
	cubeMesh.vertices.push_back(Gadget::Vector3(-1.0,  1.0,  1.0));

	cubeMesh.vertices.push_back(Gadget::Vector3( 1.0, -1.0, -1.0));
	cubeMesh.vertices.push_back(Gadget::Vector3( 1.0, -1.0,  1.0));
	cubeMesh.vertices.push_back(Gadget::Vector3( 1.0,  1.0, -1.0));
	cubeMesh.vertices.push_back(Gadget::Vector3( 1.0,  1.0,  1.0));

	cubeMesh.indices.push_back(0); cubeMesh.indices.push_back(2); cubeMesh.indices.push_back(1);
	cubeMesh.indices.push_back(1); cubeMesh.indices.push_back(2); cubeMesh.indices.push_back(3);

	cubeMesh.indices.push_back(4); cubeMesh.indices.push_back(5); cubeMesh.indices.push_back(6);
	cubeMesh.indices.push_back(6); cubeMesh.indices.push_back(5); cubeMesh.indices.push_back(7);

	cubeMesh.indices.push_back(8); cubeMesh.indices.push_back(9); cubeMesh.indices.push_back(10);
	cubeMesh.indices.push_back(10); cubeMesh.indices.push_back(9); cubeMesh.indices.push_back(11);

	cubeMesh.indices.push_back(12); cubeMesh.indices.push_back(14); cubeMesh.indices.push_back(13);
	cubeMesh.indices.push_back(14); cubeMesh.indices.push_back(15); cubeMesh.indices.push_back(13);

	cubeMesh.indices.push_back(16); cubeMesh.indices.push_back(18); cubeMesh.indices.push_back(17);
	cubeMesh.indices.push_back(17); cubeMesh.indices.push_back(18); cubeMesh.indices.push_back(19);

	cubeMesh.indices.push_back(20); cubeMesh.indices.push_back(21); cubeMesh.indices.push_back(22);
	cubeMesh.indices.push_back(21); cubeMesh.indices.push_back(23); cubeMesh.indices.push_back(22);


	cubeMesh.vertices[0].color = Gadget::Vector4(0.f, 1.f, 1.f, 1.f);
	cubeMesh.vertices[1].color = Gadget::Vector4(0.f, 1.f, 1.f, 1.f);
	cubeMesh.vertices[2].color = Gadget::Vector4(0.f, 1.f, 1.f, 1.f);
	cubeMesh.vertices[3].color = Gadget::Vector4(0.f, 1.f, 1.f, 1.f);

	// +X face
	cubeMesh.vertices[4].color = Gadget::Vector4(1.f, 0.f, 0.f, 1.f);
	cubeMesh.vertices[5].color = Gadget::Vector4(1.f, 0.f, 0.f, 1.f);
	cubeMesh.vertices[6].color = Gadget::Vector4(1.f, 0.f, 0.f, 1.f);
	cubeMesh.vertices[7].color = Gadget::Vector4(1.f, 0.f, 0.f, 1.f);

	// -Y face
	cubeMesh.vertices[8].color = Gadget::Vector4(1.f, 0.f, 1.f, 1.f);
	cubeMesh.vertices[9].color = Gadget::Vector4(1.f, 0.f, 1.f, 1.f);
	cubeMesh.vertices[10].color = Gadget::Vector4(1.f, 0.f, 1.f, 1.f);
	cubeMesh.vertices[11].color = Gadget::Vector4(1.f, 0.f, 1.f, 1.f);

	// +Y face
	cubeMesh.vertices[12].color = Gadget::Vector4(0.f, 1.f, 0.f, 1.f);
	cubeMesh.vertices[13].color = Gadget::Vector4(0.f, 1.f, 0.f, 1.f);
	cubeMesh.vertices[14].color = Gadget::Vector4(0.f, 1.f, 0.f, 1.f);
	cubeMesh.vertices[15].color = Gadget::Vector4(0.f, 1.f, 0.f, 1.f);

	// -Z face
	cubeMesh.vertices[16].color = Gadget::Vector4(1.f, 1.f, 0.f, 1.f);
	cubeMesh.vertices[17].color = Gadget::Vector4(1.f, 1.f, 0.f, 1.f);
	cubeMesh.vertices[18].color = Gadget::Vector4(1.f, 1.f, 0.f, 1.f);
	cubeMesh.vertices[19].color = Gadget::Vector4(1.f, 1.f, 0.f, 1.f);

	// +Z face
	cubeMesh.vertices[20].color = Gadget::Vector4(0.f, 0.f, 1.f, 1.f);
	cubeMesh.vertices[21].color = Gadget::Vector4(0.f, 0.f, 1.f, 1.f);
	cubeMesh.vertices[22].color = Gadget::Vector4(0.f, 0.f, 1.f, 1.f);
	cubeMesh.vertices[23].color = Gadget::Vector4(0.f, 0.f, 1.f, 1.f);

	auto aspect = screenW * 1.0 / screenH;

	auto transform = Gadget::Matrix4::Identity();
	//transform *= Gadget::Math::Scale(Gadget::Vector3(0.5, 0.5, 0.5));
	transform *= Gadget::Math::Scale(Gadget::Vector3(0.0025, 0.0025, 0.0025));
	transform *= Gadget::Math::Translate(Gadget::Vector3(0.0, 0.0, -100.0));
	transform *= Gadget::Matrix4::Perspective(Gadget::Math::Pi / 3.0, aspect, 0.01, 100.0);

	// Scale by aspect ratio
	// TODO - This is a hack that we'll address with projection matrices later
	//transform *= Gadget::Math::Scale(Gadget::Vector3(aspect, 1.0, 1.0));

	while (true)
	{
		SDL_Event e{};
		if (SDL_PollEvent(&e) && e.type == SDL_EVENT_QUIT)
		{
			break;
		}

		curTime = std::chrono::system_clock::now().time_since_epoch();
		counter.AddFrameTime(std::chrono::duration_cast<std::chrono::microseconds>(curTime - prevTime));

		double deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(curTime - prevTime).count() / 1000.0;
		transform *= Gadget::Math::Rotate(deltaTime * 25.0, Gadget::Vector3(0.0, 1.0, 0.0));
		transform *= Gadget::Math::Rotate(deltaTime * 25.0 * 1.5, Gadget::Vector3(1.0, 0.0, 0.0));

		//auto currentTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

		imageView.Lock();

		/*imageView.Clear(Gadget::Vector4(
			std::sin(static_cast<double>(currentTimeMs) / 1000.0),
			std::sin(static_cast<double>(currentTimeMs) / 1000.0 - 1),
			std::sin(static_cast<double>(currentTimeMs) / 1000.0 - 2),
			1.0
		));*/

		imageView.Clear(Gadget::Color(0.1, 0.1, 0.1));

		Draw(viewport, imageView, RS::DrawCall(cubeMesh, transform));
		//Draw(viewport, imageView, RS::DrawCall(triMesh, Gadget::Math::Translate(Gadget::Vector3(-150.0, -50.0, 0.0))));
		//Draw(viewport, imageView, RS::DrawCall(triMesh, Gadget::Math::Translate(Gadget::Vector3(150.0, 175.0, 0.0))));
		//for (int i = 0; i < 64; i++)
		//{
		//	Draw(imageView, RS::DrawCall(triMesh, Gadget::Math::Translate(Gadget::Vector3(100.0 * (i % 8), 100.0 * (i / 8), 0.0))));
		//}

		imageView.Unlock();

		SDL_UpdateWindowSurface(window);
		prevTime = curTime;
	}

	SDL_DestroyWindow(window);
	std::println("Average frame time: {:.3f}ms", counter.GetAverageFrameTimeInMicroseconds() / 1'000.0);
	return 0;
}
