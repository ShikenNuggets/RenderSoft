#pragma once

#include <GCore/Math/Math.hpp>

namespace RS
{
	class Viewport
	{
	public:
		constexpr Viewport(int32_t xMin_, int32_t xMax_, int32_t yMin_, int32_t yMax_) : xMin(xMin_), xMax(xMax_), yMin(yMin_), yMax(yMax_){}

		[[nodiscard]] inline constexpr Gadget::Vector2 NdcToViewport(const Gadget::Vector4& ndcPos) const
		{
			return Gadget::Vector2(
				xMin + (xMax - xMin) * (0.5 + 0.5 * ndcPos.x),
				yMin + (yMax - yMin) * (0.5 - 0.5 * ndcPos.y)
			);
		}

		inline constexpr int32_t GetXMin() const noexcept{ return xMin; }
		inline constexpr int32_t GetXMax() const noexcept{ return xMax; }
		inline constexpr int32_t GetYMin() const noexcept{ return yMin; }
		inline constexpr int32_t GetYMax() const noexcept{ return yMax; }

	private:
		int32_t xMin;
		int32_t xMax;
		int32_t yMin;
		int32_t yMax;
	};
}
