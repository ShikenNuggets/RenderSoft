#pragma once

#include <GCore/Math/Math.hpp>
#include <GCore/Graphics/Color.hpp>

namespace RS
{
	struct Vertex
	{
		Vertex(const Gadget::Vector4& pos = Gadget::Vector4(0.0, 0.0, 0.0, 1.0), const Gadget::Color& color_ = Gadget::Color(0.0, 0.0, 0.0, 1.0)) : position(pos), color(color_){}

		Gadget::Vector4 position;
		Gadget::Color color;
	};
}
