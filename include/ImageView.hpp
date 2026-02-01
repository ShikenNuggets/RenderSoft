#pragma once

#include <cstdint>

#include <SDL3/SDL.h>

#include <GCore/Graphics/Color.hpp>

#include "FrameBuffer.hpp"

namespace RS
{
	class ImageView
	{
	public:
		ImageView(SDL_Surface* surface_);

		void Lock();
		void Unlock();

		void Clear(const Gadget::Color& color);
		void AssignPixel(int32_t x, int32_t y, Uint32 color);
		void AssignPixel(int32_t x, int32_t y, const Gadget::Color& color);

		void CopyFrameBuffer(const RS::FrameBuffer& buffer);

		[[nodiscard]] int Width() const;
		[[nodiscard]] int Height() const;

	private:
		SDL_Surface* surface;
	};
}
