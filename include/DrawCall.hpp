#pragma once

#include <cstdint>

#include <GCore/Graphics/MeshData.hpp>
#include <GCore/Math/Matrix.hpp>

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
		DrawCall(Gadget::MeshData& mesh_, Gadget::Matrix4 transform_ = Gadget::Matrix4::Identity()) : mesh(mesh_), mode(CullMode::CCW), writeDepth(true), depthMode(DepthTestMode::Less), transform(transform_), debugCheckerboard(false){}

		Gadget::MeshData& mesh;
		CullMode mode;
		bool writeDepth;
		DepthTestMode depthMode;
		Gadget::Matrix4 transform;
		bool debugCheckerboard;
	};
}
