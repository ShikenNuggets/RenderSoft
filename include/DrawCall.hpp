#pragma once

#include <cstdint>

#include <GCore/Math/Matrix.hpp>

#include "Mesh.hpp"

namespace RS
{
	enum class CullMode : uint8_t
	{
		None,
		CW,
		CCW
	};

	class DrawCall
	{
	public:
		DrawCall(RS::Mesh& mesh_) : mesh(mesh_), mode(CullMode::CCW), transform(Gadget::Matrix4::Identity()){}

		RS::Mesh& mesh;
		CullMode mode;
		Gadget::Matrix4 transform;
	};
}
