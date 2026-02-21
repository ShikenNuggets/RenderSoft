#pragma once

#include <GCore/Graphics/Vertex.hpp>
#include <GCore/Math/Vector.hpp>

#include "DrawCall.hpp"
#include "StackVector.hpp"

namespace RS::Raster
{
	using Triangle = std::array<Gadget::Vertex, 3>;
	using ClippedTriangleList = RS::StackVector<Triangle, 12>;

	Gadget::Vertex ClipIntersectEdge(const Gadget::Vertex& v0, const Gadget::Vertex& v1, double value0, double value1);

	void ClipTriangle(const Triangle& triangle, const Gadget::Vector4& equation, ClippedTriangleList& result);

	ClippedTriangleList ClipTriangle(Triangle inTriangle);

	bool DepthTest(RS::DepthTestMode mode, uint32_t value, uint32_t reference);
}
