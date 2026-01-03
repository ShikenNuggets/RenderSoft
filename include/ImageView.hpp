#pragma once

#include <cstdint>

#include <SDL3/SDL.h>

#include <GCore/Math/Math.hpp>

namespace RS
{
	class ImageView
	{
	public:
		ImageView(SDL_Surface* surface_);

		void Lock();
		void Unlock();

		void Clear(const Gadget::Vector4& color);
		void AssignPixel(int32_t x, int32_t y, const Gadget::Vector4& color);

	private:
		SDL_Surface* surface;
	};
}
