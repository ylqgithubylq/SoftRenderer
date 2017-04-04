#pragma once

#include "BasicType.hpp"

#include <utility>
#include <array>
#include <vector>
#include <cassert>

namespace X
{
	template <typename T, u32 Dimension>
	struct Point
	{
		std::array<T, Dimension> data;
		explicit Point(std::array<T, Dimension> const& data)
			: data(data)
		{
		}
		explicit Point(T const& x)
		{
			static_assert(Dimension == 1, "");
			data[0] = x;
		}
		explicit Point(T const& x, T const& y)
		{
			static_assert(Dimension == 2, "");
			data[0] = x;
			data[1] = y;
		}
		explicit Point(T const& x, T const& y, T const& z)
		{
			static_assert(Dimension == 3, "");
			data[0] = x;
			data[1] = y;
			data[2] = z;
		}

		T const& X() const
		{
			static_assert(Dimension >= 1, "");
			return data[0];
		}
		T const& Y() const
		{
			static_assert(Dimension >= 2, "");
			return data[1];
		}
		T const& Z() const
		{
			static_assert(Dimension >= 3, "");
			return data[2];
		}

		T& operator [](u32 index)
		{
			assert(index < Dimension);
			return data[index];
		}
		T const& operator [](u32 index) const
		{
			assert(index < Dimension);
			return data[index];
		}
	};

	template <typename T, u32 Dimension>
	struct Size
	{
		std::array<T, Dimension> data;
		explicit Size(std::array<T, Dimension> const& data)
			: data(data)
		{
		}
		explicit Size(T const& x)
		{
			static_assert(Dimension == 1, "");
			data[0] = x;
		}
		explicit Size(T const& x, T const& y)
		{
			static_assert(Dimension == 2, "");
			data[0] = x;
			data[1] = y;
		}
		explicit Size(T const& x, T const& y, T const& z)
		{
			static_assert(Dimension == 3, "");
			data[0] = x;
			data[1] = y;
			data[2] = z;
		}

		T const& X() const
		{
			static_assert(Dimension >= 1, "");
			return data[0];
		}
		T const& Y() const
		{
			static_assert(Dimension >= 2, "");
			return data[1];
		}
		T const& Z() const
		{
			static_assert(Dimension >= 3, "");
			return data[2];
		}

		T& operator [](u32 index)
		{
			assert(index < Dimension);
			return data[index];
		}
		T const& operator [](u32 index) const
		{
			assert(index < Dimension);
			return data[index];
		}
	};


	template <typename T>
	struct Rectangle
	{
		T x;
		T y;
		T width;
		T height;
		Rectangle(T const& x, T const& y, T const& width, T const& height)
			: x(x), y(y), width(width), height(height)
		{
		}
	};

	template <typename T>
	void SwapBackRemove(std::vector<T>& vector, typename std::vector<T>::iterator toBeRemove)
	{
		assert(toBeRemove != vector.end());
		auto last = --vector.end();
		*toBeRemove = std::move(*last);
		vector.pop_back();
	}

	class Noncopyable
	{
	protected:
		Noncopyable()
		{
		}
		~Noncopyable()
		{
		}
	private:  // emphasize the following members are private
		Noncopyable(Noncopyable const&);
		Noncopyable& operator =(Noncopyable const&);
	};


	template <typename To, typename From>
	inline To CheckedCast(From p)
	{
		assert(dynamic_cast<To>(p) == static_cast<To>(p));
		return static_cast<To>(p);
	}

	template <typename To, typename From>
	inline std::shared_ptr<To> CheckedSPCast(std::shared_ptr<From> const& p)
	{
		assert(std::dynamic_pointer_cast<To>(p) == std::static_pointer_cast<To>(p));
		return std::static_pointer_cast<To>(p);
	}
// 
// 
// 	template <typename T>
// 	inline std::unique_ptr<T> std::make_unique()
// 	{
// 		return std::unique_ptr<T>(new T());
// 	}
// 	template <typename T, typename A0>
// 	inline std::unique_ptr<T> std::make_unique(A0&& a0)
// 	{
// 		return std::unique_ptr<T>(new T(std::forward<A0>(a0)));
// 	}
// 	template <typename T, typename A0, typename A1>
// 	inline std::unique_ptr<T> std::make_unique(A0&& a0, A1&& a1)
// 	{
// 		return std::unique_ptr<T>(new T(std::forward<A0>(a0), std::forward<A1>(a1)));
// 	}
// 	template <typename T, typename A0, typename A1, typename A2>
// 	inline std::unique_ptr<T> std::make_unique(A0&& a0, A1&& a1, A2&& a2)
// 	{
// 		return std::unique_ptr<T>(new T(std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2)));
// 	}
// 	template <typename T, typename A0, typename A1, typename A2, typename A3>
// 	inline std::unique_ptr<T> std::make_unique(A0&& a0, A1&& a1, A2&& a2, A3&& a3)
// 	{
// 		return std::unique_ptr<T>(new T(std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3)));
// 	}
// 	template <typename T, typename A0, typename A1, typename A2, typename A3, typename A4>
// 	inline std::unique_ptr<T> std::make_unique(A0&& a0, A1&& a1, A2&& a2, A3&& a3, A4&& a4)
// 	{
// 		return std::unique_ptr<T>(new T(std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3), std::forward<A4>(a4)));
// 	}
// 	template <typename T, typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
// 	inline std::unique_ptr<T> std::make_unique(A0&& a0, A1&& a1, A2&& a2, A3&& a3, A4&& a4, A5&& a5)
// 	{
// 		return std::unique_ptr<T>(new T(std::forward<A0>(a0), std::forward<A1>(a1), std::forward<A2>(a2), std::forward<A3>(a3), std::forward<A4>(a4), std::forward<A5>(a5)));
// 	}

}
