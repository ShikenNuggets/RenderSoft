#include "RenderSoft.hpp"

#include <chrono>
#include <print>

#include <GCore/ThreadPool.hpp>
#include <GCore/Window.hpp>
#include <GCore/Graphics/MeshData.hpp>
#include <GCore/Graphics/MeshLoader.hpp>

#include "DrawCall.hpp"
#include "FrameBuffer.hpp"
#include "FrameCounter.hpp"
#include "MeshAssets.hpp"
#include "StackVector.hpp"
#include "Viewport.hpp"

std::array<std::mutex, 32> depthBufferMutices;

static inline Gadget::Vertex ClipIntersectEdge(const Gadget::Vertex& v0, const Gadget::Vertex& v1, double value0, double value1)
{
	const auto t = value0 / (value0 - value1);

	return Gadget::Vertex(
		(1.0 - t) * v0.position + t * v1.position,
		(1.0f - static_cast<float>(t)) * v0.color + static_cast<float>(t) * v1.color
	);
}

using Triangle = std::array<Gadget::Vertex, 3>;
using ClippedTriangleList = RS::StackVector<Triangle, 12>;

static inline void ClipTriangle(const Triangle& triangle, const Gadget::Vector4& equation, ClippedTriangleList& result)
{
	std::array<double, 3> values =
	{
		Gadget::Vector4::Dot(triangle[0].position, equation),
		Gadget::Vector4::Dot(triangle[1].position, equation),
		Gadget::Vector4::Dot(triangle[2].position, equation)
	};

	const uint8_t mask = (values[0] < 0.0 ? 1 : 0) | (values[1] < 0.0 ? 2 : 0) | (values[2] < 0.0 ? 4 : 0);
	switch (mask)
	{
		case 0b000:
			// All vertices are inside allowed half-space
			// No clipping required, copy the triangle to output
			result.push_back(triangle);
			break;
		case 0b001:
			// Vertex 0 is outside allowed half-space
			// Replace it with points on edges 01 and 02
			// And re-triangulate
			{
				auto v01 = ClipIntersectEdge(triangle[0], triangle[1], values[0], values[1]);
				auto v02 = ClipIntersectEdge(triangle[0], triangle[2], values[0], values[2]);
				result.push_back({ v01, triangle[1], triangle[2] });
				result.push_back({ v01, triangle[2], v02 });
			}
			break;
		case 0b010:
			// Vertex 1 is outside allowed half-space
			// Replace it with points on edges 10 and 12
			// And re-triangulate
			{
				auto v10 = ClipIntersectEdge(triangle[1], triangle[0], values[1], values[0]);
				auto v12 = ClipIntersectEdge(triangle[1], triangle[2], values[1], values[2]);
				result.push_back({ triangle[0], v10, triangle[2]});
				result.push_back({ triangle[2], v10, v12});
			}
			break;
		case 0b011:
			// Vertices 0 and 1 are outside allowed half-space
			// Replace them with points on edges 02 and 12
			{
				auto v02 = ClipIntersectEdge(triangle[0], triangle[2], values[0], values[2]);
				auto v12 = ClipIntersectEdge(triangle[1], triangle[2], values[1], values[2]);
				result.push_back({ v02, v12, triangle[2] });
			}
			break;
		case 0b100:
			// Vertex 2 is outside allowed half-space
			// Replace it with points on edges 20 and 21
			// And re-triangulate
			{
				auto v20 = ClipIntersectEdge(triangle[2], triangle[0], values[2], values[0]);
				auto v21 = ClipIntersectEdge(triangle[2], triangle[1], values[2], values[1]);
				result.push_back({ triangle[0], triangle[1], v20});
				result.push_back({ v20, triangle[1], v21});
			}
			break;
		case 0b101:
			// Vertices 0 and 2 are outside allowed half-space
			// Replace them with points on edges 01 and 21
			{
				auto v01 = ClipIntersectEdge(triangle[0], triangle[1], values[0], values[1]);
				auto v21 = ClipIntersectEdge(triangle[2], triangle[1], values[2], values[1]);
				result.push_back({ v01, triangle[1], v21 });
			}
			break;
		case 0b110:
			// Vertices 1 and 2 are outside allowed half-space
			// Replace them with points on edges 10 and 20
			{
				auto v10 = ClipIntersectEdge(triangle[1], triangle[0], values[1], values[0]);
				auto v20 = ClipIntersectEdge(triangle[2], triangle[0], values[2], values[0]);
				result.push_back({ triangle[0], v10, v20 });
			}
			break;
		case 0b111:
			// All vertices are outside allowed half-space
			// Clip the whole triangle, result is empty
			break;
	}
}

static inline ClippedTriangleList ClipTriangle(Triangle inTriangle)
{
	static const std::array<Gadget::Vector4, 2> equations =
	{
		Gadget::Vector4(0.0, 0.0, 1.0, 1.0),
		Gadget::Vector4(0.0, 0.0, -1.0, 1.0)
	};

	ClippedTriangleList eq1Result;
	eq1Result.reserve(12);

	// Equation 1 - only one triangle to clip
	ClipTriangle(inTriangle, equations[0], eq1Result);

	// Equation 2 - possibly more than one triangle
	ClippedTriangleList eq2Result;
	eq2Result.reserve(12);

	for (const auto& tri : eq1Result)
	{
		ClipTriangle(tri, equations[1], eq2Result);
	}

	return eq2Result;
}

static inline bool DepthTest(RS::DepthTestMode mode, uint32_t value, uint32_t reference)
{
	switch (mode)
	{
		case RS::DepthTestMode::Always:
			return true;
		case RS::DepthTestMode::Never:
			return false;
		case RS::DepthTestMode::Less:
			return value < reference;
		case RS::DepthTestMode::LessEqual:
			return value <= reference;
		case RS::DepthTestMode::Greater:
			return value > reference;
		case RS::DepthTestMode::GreaterEqual:
			return value >= reference;
		case RS::DepthTestMode::Equal:
			return value == reference;
		case RS::DepthTestMode::NotEqual:
			return value != reference;
	}

	return true;
}

static void Rasterize(const Triangle& tri, const RS::Viewport& viewport, RS::FrameBuffer& frameBuffer, const RS::DrawCall& drawCall)
{
	auto vert0 = tri[0];
	auto vert1 = tri[1];
	auto vert2 = tri[2];

	auto projVert0 = vert0.position / vert0.position.w;
	auto projVert1 = vert1.position / vert1.position.w;
	auto projVert2 = vert2.position / vert2.position.w;

	auto v0 = viewport.NdcToViewport(projVert0);
	auto v1 = viewport.NdcToViewport(projVert1);
	auto v2 = viewport.NdcToViewport(projVert2);

	auto det012 = Gadget::Vector2::Determinant(v1 - v0, v2 - v0);
	if (Gadget::Math::IsNearZero(det012))
	{
		return; // Early out, triangle is very very small
	}

	const bool ccw = det012 < 0.0;
	if (ccw && drawCall.mode == RS::CullMode::CW || !ccw && drawCall.mode == RS::CullMode::CCW)
	{
		return; // Skip this triangle (back-face culling)
	}

	if (ccw)
	{
		std::swap(vert1, vert2);
		std::swap(projVert1, projVert2);
		std::swap(v1, v2);
		det012 = -det012;
	}

	auto c0 = vert0.color;
	auto c1 = vert1.color;
	auto c2 = vert2.color;

	std::array<Gadget::Vector2, 3> verts = { v0, v1, v2 };
	auto bounds = Gadget::Math::CalculateBounds<double>(verts); // TODO - span was a nice idea, but the dev UX here kinda blows

	if (bounds.min.x >= frameBuffer.Width() || bounds.min.y >= frameBuffer.Height() || bounds.max.x < 0.0 || bounds.max.y < 0.0)
	{
		return; // Early out, triangle bounds are fully off-screen
	}

	// Ignore any part of the bounds that are off-screen
	bounds.min.x = std::max<double>(bounds.min.x, viewport.GetXMin());
	bounds.min.y = std::max<double>(bounds.min.y, viewport.GetYMin());
	bounds.max.x = std::min<double>(std::min<double>(bounds.max.x, frameBuffer.Width() - 1), viewport.GetXMax());
	bounds.max.y = std::min<double>(std::min<double>(bounds.max.y, frameBuffer.Height() - 1), viewport.GetYMax());

	for (int y = bounds.min.y; y < bounds.max.y; y++)
	{
		for (int x = bounds.min.x; x < bounds.max.x; x++)
		{
			auto p = Gadget::Vector2(static_cast<double>(x) + 0.5, static_cast<double>(y) + 0.5);

			const auto det01p = Gadget::Vector2::Determinant(v1 - v0, p - v0);
			const auto det12p = Gadget::Vector2::Determinant(v2 - v1, p - v1);
			const auto det20p = Gadget::Vector2::Determinant(v0 - v2, p - v2);

			if (det01p >= 0.0 && det12p >= 0.0 && det20p >= 0.0)
			{
				auto l0 = det12p / det012;
				auto l1 = det20p / det012;
				auto l2 = det01p / det012;

				const auto z = (l0 * projVert0.z) + (l1 * projVert1.z) + (l2 * projVert2.z);
				const uint32_t depth = (0.5 + 0.5 * z) * std::numeric_limits<uint32_t>::max();

				{
					const auto pixelIdx = (y * frameBuffer.Height()) + x;
					auto lock = std::lock_guard(depthBufferMutices.at(pixelIdx % depthBufferMutices.size()));
					if (!DepthTest(drawCall.depthMode, depth, frameBuffer.depth.GetPixel(x, y)))
					{
						continue;
					}

					if (drawCall.writeDepth)
					{
						frameBuffer.depth.SetPixel(x, y, depth);
					}
				}

				l0 /= det012;
				l1 /= det012;
				l2 /= det012;
				const auto lSum = l0 + l1 + l2;
				l0 /= lSum;
				l1 /= lSum;
				l2 /= lSum;

				auto finalColor = (c0 * l0) + (c1 * l1) + (c2 * l2);
				if (drawCall.debugCheckerboard)
				{
					if (static_cast<int>(std::floor(finalColor.r * 8.0) + std::floor(finalColor.g * 8.0)) % 2 == 0)
					{
						finalColor = { 0, 0, 0, 255 };
					}
					else
					{
						finalColor = { 255, 255, 255, 255 };
					}
				}

				frameBuffer.color.SetPixel(x, y, finalColor);
			}
		}
	}
}

static void Draw(const RS::Viewport& viewport, RS::FrameBuffer& frameBuffer, const RS::DrawCall& drawCall)
{
	static Gadget::ThreadPool threadPool{};

	for (size_t i = 0; i + 2 < drawCall.mesh.indices.size(); i += 3)
	{
		threadPool.QueueJob([&, i]()
		{
			const auto i0 = drawCall.mesh.indices[i];
			const auto i1 = drawCall.mesh.indices[i + 1];
			const auto i2 = drawCall.mesh.indices[i + 2];

			auto clip0 = drawCall.transform * drawCall.mesh.vertices[i0].position;
			auto clip1 = drawCall.transform * drawCall.mesh.vertices[i1].position;
			auto clip2 = drawCall.transform * drawCall.mesh.vertices[i2].position;

			const auto clipVert0 = Gadget::Vertex(clip0, drawCall.mesh.vertices[i0].color);
			const auto clipVert1 = Gadget::Vertex(clip1, drawCall.mesh.vertices[i1].color);
			const auto clipVert2 = Gadget::Vertex(clip2, drawCall.mesh.vertices[i2].color);

			// To disable view clipping, just rasterize the triangle directly
			//Rasterize({ clipVert0, clipVert1, clipVert2 }, viewport, frameBuffer, drawCall);

			const auto clippedTris = ClipTriangle({ clipVert0, clipVert1, clipVert2 });

			for (const auto& tri : clippedTris)
			{
				Rasterize(tri, viewport, frameBuffer, drawCall);
			}
		});
	}

	threadPool.Start();
	while (threadPool.IsBusy())
	{
		continue;
	}
	threadPool.Stop();
}

void CopyFrameBuffer(Gadget::WindowSurfaceView& surfaceView, const RS::FrameBuffer& buffer)
{
	for (uint16_t x = 0; x < buffer.Width(); x++)
	{
		for (uint16_t y = 0; y < buffer.Height(); y++)
		{
			surfaceView.AssignPixel(x, y, buffer.color.GetPixel(x, y));
		}
	}
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[])
{
	static constexpr auto screenW = 800;
	static constexpr auto screenH = 600;

	std::println("Hello, World!");

	auto window = std::make_unique<Gadget::Window>(screenW, screenH, Gadget::RenderAPI::None, "Software RasterMan");
	auto surfaceView = window->GetSurfaceView();

	RS::FrameCounter counter;

	auto prevTime = std::chrono::system_clock::now().time_since_epoch();
	auto curTime = prevTime;

	auto frameBuffer = RS::FrameBuffer(screenW, screenH);

	auto viewport = RS::Viewport(0, screenW, 0, screenH);

	auto rectMesh = RS::GetRectMesh();
	auto cubeMesh = RS::GetCubeMesh();
	auto testModel = Gadget::MeshLoader::LoadMeshFromFile("assets\\teapot.stl");
	const auto& testMesh = testModel.meshes[0];

	auto aspect = screenW * 1.0 / screenH;

	auto transform = Gadget::Matrix4::Identity();

	auto pos = Gadget::Vector3(0.0, 0.0, -10.0);
	auto rot = Gadget::Euler(0.0, 0.0, 0.0);
	auto scale = Gadget::Vector3(0.5, 0.5, 0.5);

	bool shouldContinue = true;
	auto quitDelegateHandle = window->EventHandler().OnQuitRequested.Add([&shouldContinue]()
	{
		shouldContinue = false;
	});

	while (shouldContinue)
	{
		window->HandleEvents();

		curTime = std::chrono::system_clock::now().time_since_epoch();
		counter.AddFrameTime(std::chrono::duration_cast<std::chrono::microseconds>(curTime - prevTime));

		const auto deltaTime = static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(curTime - prevTime).count()) / 1000.0;
		rot = rot + Gadget::Euler(deltaTime * 25.0 * 1.5, deltaTime * 25.0, 0.0);
		//rot = rot + Gadget::Euler(deltaTime * 25.0 * 1.5, 0.0, 0.0);

		const auto positionMatrix = Gadget::Math::Translate(pos);
		const auto rotationMatrix = Gadget::Math::ToMatrix4(Gadget::Math::ToQuaternion(rot));
		const auto scaleMatrix = Gadget::Math::Scale(scale);

		transform = (positionMatrix * (rotationMatrix * scaleMatrix));
		transform = Gadget::Matrix4::Perspective(90.0, aspect, 0.01, 1000.0) * transform;

		frameBuffer.Clear();
		Draw(viewport, frameBuffer, RS::DrawCall(testMesh, transform));

		surfaceView.Lock();
		surfaceView.Clear(Gadget::Color(0.1f, 0.1f, 0.1f));
		CopyFrameBuffer(surfaceView, frameBuffer);
		surfaceView.Unlock();

		window->UpdateWindowSurface();
		prevTime = curTime;
	}

	window->EventHandler().OnQuitRequested.Remove(quitDelegateHandle);

	window.reset();
	std::println("Average frame time: {:.3f}ms", counter.GetAverageFrameTimeInMicroseconds() / 1'000.0);
	return 0;
}
