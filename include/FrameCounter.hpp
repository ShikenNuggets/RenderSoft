#pragma once

#include <chrono>
#include <numeric>

#include <GCore/Data/RingBuffer.hpp>

namespace RS
{
	class FrameCounter
	{
	public:
		FrameCounter(){};

		void AddFrameTime(std::chrono::microseconds time)
		{
			buffer.Add(time.count());
		}

		[[nodiscard]] double GetAverageFrameTimeInMicroseconds() const
		{
			int64_t total = std::reduce(buffer.begin(), buffer.end(), int64_t{ 0 });
			return static_cast<double>(total) / BufferSize;
		}

	private:
		static constexpr int64_t BufferSize = 15;
		Gadget::RingBuffer<int64_t, BufferSize> buffer;
	};
}
