#include "ImageView.hpp"

#include <print>

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

void ImageView::AssignPixel(int32_t x, int32_t y, Uint32 color)
{
	if (surface == nullptr)
	{
		std::println("ImageView has no valid surface, cannot assign color to pixel");
		return;
	}

	if (x < 0 || y < 0 || x >= surface->w || y >= surface->h)
	{
		std::println("Tried to assign color to invalid pixel ({},{}), ImageView size is ({},{})", x, y, surface->w, surface->h);
		return;
	}

	const auto pixels = static_cast<uint32_t*>(surface->pixels);
	const auto pitchInPixels = surface->pitch / sizeof(surface->pitch);

	pixels[y * pitchInPixels + x] = color;
}

void ImageView::AssignPixel(int32_t x, int32_t y, const Gadget::Vector4& color)
{
	Gadget::Vector4 sRGB;

	if (color.x < 0.00313066844250063)
	{
		sRGB.x = color.x * 12.92;
	}
	else
	{
		sRGB.x = 1.055 * (std::pow(color.x, 1.0 / 2.4)) - 0.055;
	}

	if (color.y < 0.00313066844250063)
	{
		sRGB.y = color.y * 12.92;
	}
	else
	{
		sRGB.y = 1.055 * (std::pow(color.y, 1.0 / 2.4)) - 0.055;
	}

	if (color.z < 0.00313066844250063)
	{
		sRGB.z = color.z * 12.92;
	}
	else
	{
		sRGB.z = 1.055 * (std::pow(color.z, 1.0 / 2.4)) - 0.055;
	}

	const auto finalColor = SDL_MapSurfaceRGB(surface, sRGB.x * 255, sRGB.y * 255, sRGB.z * 255);
	AssignPixel(x, y, finalColor);
}

int ImageView::Width() const
{
	if (surface == nullptr)
	{
		return -1;
	}

	return surface->w;
}

int ImageView::Height() const
{
	if (surface == nullptr)
	{
		return -1;
	}

	return surface->h;
}
