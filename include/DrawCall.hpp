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

	enum class DepthTestMode
	{
		Never,
		Always,
		Less,
		LessEqual,
		Greater,
		GreaterEqual,
		Equal,
		NotEqual
	};

	class DrawCall
	{
	public:
		DrawCall(RS::Mesh& mesh_, Gadget::Matrix4 transform_ = Gadget::Matrix4::Identity()) : mesh(mesh_), mode(CullMode::None), writeDepth(true), depthMode(DepthTestMode::Less), transform(transform_), debugCheckerboard(false){}

		RS::Mesh& mesh;
		CullMode mode;
		bool writeDepth;
		DepthTestMode depthMode;
		Gadget::Matrix4 transform;
		bool debugCheckerboard;
	};
}
