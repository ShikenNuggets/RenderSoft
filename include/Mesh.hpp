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
		std::vector<size_t> indices;
	};
}
