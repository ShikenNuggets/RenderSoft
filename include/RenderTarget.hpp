#pragma once

#include <cstdint>
#include <print>
#include <vector>

namespace RS
{
	template <typename Pixel>
	class RenderTarget
	{
	public:
		RenderTarget(uint16_t width_, uint16_t height_, Pixel default_) : width(width_), height(height_)
		{
			pixels.resize(width * height, default_);
		}

		void Clear(const Pixel& color)
		{
			for (auto& pixel : pixels)
			{
				pixel = color;
			}
		}

		const Pixel& GetPixel(uint16_t x, uint16_t y) const
		{
			return pixels[GetPixelIndex(x, y)];
		}

		void SetPixel(uint16_t x, uint16_t y, const Pixel& color)
		{
			if (x < 0 || y < 0 || x >= width || y >= height)
			{
				std::println("Tried to assign color to invalid pixel ({},{}), RenderTarget size is ({},{})", x, y, width, height);
				return;
			}

			pixels[GetPixelIndex(x, y)] = color;
		}

		uint32_t GetPixelIndex(uint16_t x, uint16_t y) const
		{
			return (static_cast<uint32_t>(y) * width) + x;
		}

		std::vector<Pixel>& GetPixels(){ return pixels; }
		const std::vector<Pixel>& GetPixels() const{ return pixels; }

	private:
		std::vector<Pixel> pixels;
		uint16_t width;
		uint16_t height;
	};
}
