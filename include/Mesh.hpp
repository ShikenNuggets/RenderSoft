#pragma once

#include <vector>

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
