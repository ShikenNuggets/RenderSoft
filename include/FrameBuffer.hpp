#pragma once

#include <cstdint>

#include <GCore/Graphics/Color.hpp>

#include "RenderTarget.hpp"

namespace RS
{
	class FrameBuffer
	{
	public:
		using DepthT = uint32_t;

		FrameBuffer(uint16_t width_, uint16_t height_) : width(width_), height(height_), color(width_, height_, Gadget::Color(0.0, 0.0, 0.0)), depth(width_, height_, std::numeric_limits<DepthT>::max()){}

		RenderTarget<Gadget::Color> color;
		RenderTarget<DepthT> depth;

		void Clear(const Gadget::Color& color_ = Gadget::Color(0.0, 0.0, 0.0), DepthT depth_ = std::numeric_limits<DepthT>::max())
		{
			color.Clear(color_);
			depth.Clear(depth_);
		}

		uint16_t Width() const{ return width; }
		uint16_t Height() const{ return height; }

	private:
		uint16_t width;
		uint16_t height;
	};
}
