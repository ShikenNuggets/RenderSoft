#pragma once

#include <cstdint>

#include <GCore/Graphics/Color.hpp>

#include "RenderTarget.hpp"

namespace RS
{
	class FrameBuffer
	{
	public:
		FrameBuffer(uint16_t width_, uint16_t height_) : width(width_), height(height_), colorBuffer(width_, height_, Gadget::Color(0.0, 0.0, 0.0)), depthBuffer(width_, height_, 0){}

		RenderTarget<Gadget::Color> colorBuffer;
		RenderTarget<uint32_t> depthBuffer;

		uint16_t Width() const{ return width; }
		uint16_t Height() const{ return height; }

	private:
		uint16_t width;
		uint16_t height;
	};
}
