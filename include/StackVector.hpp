#pragma once

#include <array>
#include <cstdint>

#include <GCore/Assert.hpp>

namespace RS
{
	// A std::array with vector semantics
	// Or alternatively, a std::vector with a fixed capacity
	template <typename T, size_t Capacity>
	class StackVector
	{
	public:
		class Iterator{
		public:
			Iterator(StackVector<T, Capacity>& data_, int64_t index_) : data(data_), index(index_){}

			inline const T& operator*() const{ return data[index]; }
			inline T& operator*(){ return data[index]; }

			inline Iterator& operator++(){ index++; return *this; }
			inline Iterator& operator--(){ index--; return *this; }
			inline bool operator!=(const Iterator& it_) const{ return index != it_.index; }

			inline int64_t Index() const{ return index; }

		private:
			StackVector<T, Capacity>& data;
			int64_t index;
		};

		class ConstIterator{
		public:
			ConstIterator(const StackVector<T, Capacity>& data_, int64_t index_) : data(data_), index(index_){}

			inline const T& operator*() const{ return data[index]; }

			inline ConstIterator& operator++(){ index++; return *this; }
			inline ConstIterator& operator--(){ index--; return *this; }
			inline bool operator!=(const ConstIterator& it_) const{ return index != it_.index; }

			inline int64_t Index() const{ return index; }

		private:
			const StackVector<T, Capacity>& data;
			int64_t index;
		};

		bool push_back(const T& value)
		{
			if (size >= Capacity)
			{
				return false;
			}
			
			internalArray[size] = value;
			size++;
			return true;
		}

		bool push_back(T&& value)
		{
			if (size >= Capacity)
			{
				return false;
			}

			GADGET_BASIC_ASSERT(size < Capacity);
			internalArray[size] = std::move(value);
			size++;
			return true;
		}

		// Intentional no-op so this can serve as a drop-in replacement for std::vector
		void reserve(size_t /* capacity */){ return; }

		const T& operator[](int64_t i) const
		{
			GADGET_BASIC_ASSERT(i < Capacity);
			return internalArray[i];
		}

		T& operator[](int64_t i)
		{
			GADGET_BASIC_ASSERT(i < Capacity);
			return internalArray[i];
		}

		Iterator begin(){ return Iterator(*this, 0); }
		ConstIterator begin() const{ return ConstIterator(*this, 0); }
		Iterator end(){ return Iterator(*this, Capacity); }
		ConstIterator end() const{ return ConstIterator(*this, Capacity); }

	private:
		std::array<T, Capacity> internalArray;
		size_t size = 0;
	};
}
