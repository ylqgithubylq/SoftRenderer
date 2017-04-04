#pragma once

#include <cmath>

#include "BasicType.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"

namespace X
{
	f64 const D_PI = 3.14159265358979323846264338327950288419716939937510582097494459230781640628620;
	//f64 const D_PI = std::atan(1.0) * 4;
	f32 const PI = static_cast<f32>(D_PI);
	f32 const RadianToDegree = static_cast<f32>(180.0 / D_PI);
	f32 const DegreeToRadian = static_cast<f32>(D_PI / 180.0);

	inline f32 RadianFromDegree(f32 degree)
	{
		return DegreeToRadian * degree;
	}
	inline f32 RadianFromDegree(s32 degree)
	{
		return DegreeToRadian * degree;
	}


	inline f32 DegreeFromRadian(f32 radian)
	{
		return RadianToDegree * radian;
	}
	inline f32 DegreeFromRadian(s32 radian)
	{
		return RadianToDegree * radian;
	}

	template <typename T>
	inline bool Equal(T const& left, T const& right)
	{
		return left == right;
	}
	template <>
	inline bool Equal<f32>(f32 const& left, f32 const& right)
	{
		return std::abs(left - right) <= std::numeric_limits<f32>::epsilon();
	}


	template <typename T>
	inline T Square(T number)
	{
		return number * number;
	}

	inline f32 InverseSqrt(f32 value)
	{
		return 1 / std::sqrt(value);
	}

	template <typename T>
	inline T Clamp(T const& value, T const& low, T const& high)
	{
		return std::max(low, std::min(high, value));
	}

	template <typename T>
	inline T Lerp(T const& left, T const& right, f32 s)
	{
		return left + (right - left) * s;
	}

	template <typename T>
	inline T Lerp3(T const& v0, T const& v1, T const& v2, f32 t0, f32 t1, f32 t2)
	{
		return v0 * t0 + v1 * t1 + v2 * t2;
	}

	/*
	*	@return: angle in [0, PI] in radians.
	*/
	inline f32 Angle(f32V3 const& from, f32V3 const& to)
	{
		return std::acos(Dot(from, to) / (from.Length() * to.Length()));
	}

	/*
	*	for 3 dimension vector, affine division will be done on the result vector.
	*/
	f32V3 Transform(f32V3 const& vector, f32 lastComponent, f32M44 const& matrix);
	/*
	*	for 3 dimension vector, affine division will be done on the result vector.
	*/
	f32V3 Transform(f32V3 const& vector, f32M44 const& matrix);

	f32V4 Transform(f32V4 const& vector, f32M44 const& matrix);

	/*
	*	4th component will be ignored and use 0 instead.
	*/
	f32V4 TransformDirection(f32V4 const& vector, f32M44 const& matrix);
	f32V3 TransformDirection(f32V3 const& vector, f32M44 const& matrix);



	f32M44 TranslationMatrix(f32 x, f32 y, f32 z);
	f32M44 TranslationMatrix(f32V3 const& v);

	f32M44 ScalingMatrix(f32 s);
	f32M44 ScalingMatrix(f32 sx, f32 sy, f32 sz);
	f32M44 ScalingMatrix(f32V3 const& s);

	f32M44 RotationMatrix(f32 angle, f32 x, f32 y, f32 z);
	f32M44 RotationMatrix(f32 angle, f32V3 axis);

	f32M44 RotationMatrixFromTo(f32V3 const& from, f32V3 const& to);

	/*
	*	Create a view matrix of camera. With assumption of camera front is +z.
	*	@eye: eye position.
	*	@to: look to direction.
	*	@up: up direction in world space.
	*	@return: view matrix.
	*/
	f32M44 LookToViewMatrix(f32V3 const& eye, f32V3 const& to, f32V3 const& up);


	f32M44 FrustumProjectionMatrix(f32 fieldOfView, f32 aspectRatio, f32 near, f32 far);
	f32M44 FrustumProjectionMatrix(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far);

}
