#include "ImageView.hpp"

using namespace RS;

ImageView::ImageView(SDL_Surface* surface_) : surface(surface_)
{
	if(surface == nullptr)
	{
		throw - 1;
	}
}

void ImageView::Lock()
{
	SDL_LockSurface(surface);
}

void ImageView::Unlock()
{
	SDL_UnlockSurface(surface);
}

void ImageView::Clear(const Gadget::Vector4& color)
{
	const auto pixels = static_cast<uint32_t*>(surface->pixels);
	const auto pitchInPixels = surface->pitch / sizeof(surface->pitch);
	const auto finalColor = SDL_MapSurfaceRGB(surface, color.x * 255, color.y * 255, color.z * 255);

	for (int y = 0; y < surface->h; y++)
	{
		for (int x = 0; x < surface->w; x++)
		{
			pixels[y * pitchInPixels + x] = finalColor;
		}
	}
}

void ImageView::AssignPixel(int32_t x, int32_t y, const Gadget::Vector4& color)
{
	const auto pixels = static_cast<uint32_t*>(surface->pixels);
	const auto pitchInPixels = surface->pitch / sizeof(surface->pitch);
	const auto finalColor = SDL_MapSurfaceRGB(surface, color.x * 255, color.y * 255, color.z * 255);

	pixels[y * pitchInPixels + x] = finalColor;
}