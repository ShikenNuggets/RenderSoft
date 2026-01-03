#pragma once

#include <vector>

#include <GCore/Math/Math.hpp>

#include "Vertex.hpp"

namespace RS
{
	class Mesh
	{
	public:
		std::vector<Vertex> vertices;
		Gadget::Vector4 color;
	};
}
