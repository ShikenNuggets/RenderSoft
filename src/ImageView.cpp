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

void ImageView::Clear(const Gadget::Color& color)
{
	const auto pixels = static_cast<uint32_t*>(surface->pixels);
	const auto pitchInPixels = surface->pitch / sizeof(surface->pitch);
	const auto finalColor = SDL_MapSurfaceRGB(surface, color.r * 255, color.g * 255, color.b * 255);

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

void ImageView::AssignPixel(int32_t x, int32_t y, const Gadget::Color& color)
{
	const Gadget::Color sRGB = color.ToSRGB();
	const auto finalColor = SDL_MapSurfaceRGB(surface, sRGB.r * 255, sRGB.g * 255, sRGB.b * 255);
	AssignPixel(x, y, finalColor);
}

void ImageView::CopyFrameBuffer(const RS::FrameBuffer& buffer)
{
	for (uint16_t x = 0; x < buffer.Width(); x++)
	{
		for (uint16_t y = 0; y < buffer.Height(); y++)
		{
			AssignPixel(x, y, buffer.color.GetPixel(x, y));
		}
	}
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
