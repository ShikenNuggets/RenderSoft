#pragma once

#include <GCore/Math/Math.hpp>
#include <GCore/Graphics/Color.hpp>

namespace RS
{
	struct Vertex
	{
		Vertex(const Gadget::Vector3& pos, const Gadget::Color& color_ = Gadget::Color(0.0, 0.0, 0.0, 1.0)) : position(pos), color(color_){}

		Gadget::Vector3 position;
		Gadget::Color color;
	};
}
