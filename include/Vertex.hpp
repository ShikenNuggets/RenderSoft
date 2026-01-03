#pragma once

#include <GCore/Math/Math.hpp>

namespace RS
{
	struct Vertex
	{
		Vertex(const Gadget::Vector3 pos) : position(pos){}

		Gadget::Vector3 position;
	};
}
