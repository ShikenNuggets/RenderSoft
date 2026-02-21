#include "Raster.hpp"

using namespace RS;

Gadget::Vertex Raster::ClipIntersectEdge(const Gadget::Vertex& v0, const Gadget::Vertex& v1, double value0, double value1)
{
	const auto t = value0 / (value0 - value1);

	return Gadget::Vertex(
		(1.0 - t) * v0.position + t * v1.position,
		(1.0f - static_cast<float>(t)) * v0.color + static_cast<float>(t) * v1.color
	);
}

void Raster::ClipTriangle(const Triangle& triangle, const Gadget::Vector4& equation, ClippedTriangleList& result)
{
	std::array<double, 3> values =
	{
		Gadget::Vector4::Dot(triangle[0].position, equation),
		Gadget::Vector4::Dot(triangle[1].position, equation),
		Gadget::Vector4::Dot(triangle[2].position, equation)
	};

	const uint8_t mask = (values[0] < 0.0 ? 1 : 0) | (values[1] < 0.0 ? 2 : 0) | (values[2] < 0.0 ? 4 : 0);
	switch(mask)
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
			result.push_back({ triangle[0], v10, triangle[2] });
			result.push_back({ triangle[2], v10, v12 });
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
			result.push_back({ triangle[0], triangle[1], v20 });
			result.push_back({ v20, triangle[1], v21 });
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

Raster::ClippedTriangleList Raster::ClipTriangle(Triangle inTriangle)
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

	for(const auto& tri : eq1Result)
	{
		ClipTriangle(tri, equations[1], eq2Result);
	}

	return eq2Result;
}

bool Raster::DepthTest(RS::DepthTestMode mode, uint32_t value, uint32_t reference)
{
	switch(mode)
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
