#pragma once

#include <GCore/Math/Math.hpp>

namespace RS
{
	class Viewport
	{
	public:
		Viewport(int32_t xMin_, int32_t xMax_, int32_t yMin_, int32_t yMax_) : xMin(xMin_), xMax(xMax_), yMin(yMin_), yMax(yMax_){}

		Gadget::Vector2 NdcToViewport(const Gadget::Vector3& ndcPos) const
		{
			return Gadget::Vector2(
				xMin + (xMax - xMin) * (0.5 + 0.5 * ndcPos.x),
				yMin + (yMax - yMin) * (0.5 - 0.5 * ndcPos.y)
			);
		}

	private:
		int32_t xMin;
		int32_t xMax;
		int32_t yMin;
		int32_t yMax;
	};
}
