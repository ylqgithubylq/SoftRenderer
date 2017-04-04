#include "Header.hpp"
#include "Math.hpp"

namespace X
{

	f32V3 Transform(f32V3 const& vector, f32 lastComponent, f32M44 const& matrix)
	{
		f32 inverseW = f32(1) / (matrix[3] * vector[0] + matrix[7] * vector[1] + matrix[11] * vector[2] + matrix[15] * lastComponent);
		return f32V3(
			(matrix[0] * vector[0] + matrix[4] * vector[1] + matrix[8] * vector[2] + matrix[12] * lastComponent) * inverseW,
			(matrix[1] * vector[0] + matrix[5] * vector[1] + matrix[9] * vector[2] + matrix[13] * lastComponent) * inverseW,
			(matrix[2] * vector[0] + matrix[6] * vector[1] + matrix[10] * vector[2] + matrix[14] * lastComponent) * inverseW);
	}

	f32V3 Transform(f32V3 const& vector, f32M44 const& matrix)
	{
		f32 inverseW = 1.f / (matrix[3] * vector[0] + matrix[7] * vector[1] + matrix[11] * vector[2] + matrix[15]);
		return f32V3(
			(matrix[0] * vector[0] + matrix[4] * vector[1] + matrix[8] * vector[2] + matrix[12]) * inverseW,
			(matrix[1] * vector[0] + matrix[5] * vector[1] + matrix[9] * vector[2] + matrix[13]) * inverseW,
			(matrix[2] * vector[0] + matrix[6] * vector[1] + matrix[10] * vector[2] + matrix[14]) * inverseW);
	}

	f32V4 Transform(f32V4 const& vector, f32M44 const& matrix)
	{
		return f32V4(
			matrix[0] * vector[0] + matrix[4] * vector[1] + matrix[8] * vector[2] + matrix[12] * vector[3],
			matrix[1] * vector[0] + matrix[5] * vector[1] + matrix[9] * vector[2] + matrix[13] * vector[3],
			matrix[2] * vector[0] + matrix[6] * vector[1] + matrix[10] * vector[2] + matrix[14] * vector[3],
			matrix[3] * vector[0] + matrix[7] * vector[1] + matrix[11] * vector[2] + matrix[15] * vector[3]);
	}

	f32V4 TransformDirection(f32V4 const& vector, f32M44 const& matrix)
	{
		return f32V4(
			matrix[0] * vector[0] + matrix[4] * vector[1] + matrix[8] * vector[2],
			matrix[1] * vector[0] + matrix[5] * vector[1] + matrix[9] * vector[2],
			matrix[2] * vector[0] + matrix[6] * vector[1] + matrix[10] * vector[2],
			0.f);
	}

	f32V3 TransformDirection(f32V3 const& vector, f32M44 const& matrix)
	{
		return f32V3(
			matrix[0] * vector[0] + matrix[4] * vector[1] + matrix[8] * vector[2],
			matrix[1] * vector[0] + matrix[5] * vector[1] + matrix[9] * vector[2],
			matrix[2] * vector[0] + matrix[6] * vector[1] + matrix[10] * vector[2]);
	}

	f32M44 TranslationMatrix(f32 x, f32 y, f32 z)
	{
		return f32M44(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			x, y, z, 1);
	}

	f32M44 TranslationMatrix(f32V3 const& v)
	{
		return TranslationMatrix(v.X(), v.Y(), v.Z());
	}

	f32M44 ScalingMatrix(f32 s)
	{
		return ScalingMatrix(s, s, s);
	}

	f32M44 ScalingMatrix(f32 sx, f32 sy, f32 sz)
	{
		return f32M44(
			sx, 0, 0, 0,
			0, sy, 0, 0,
			0, 0, sz, 0,
			0, 0, 0, 1);
	}

	f32M44 ScalingMatrix(f32V3 const& s)
	{
		return ScalingMatrix(s.X(), s.Y(), s.Z());
	}

	f32M44 RotationMatrix(f32 angle, f32 x, f32 y, f32 z)
	{
		return RotationMatrix(angle, f32V3(x, y, z));
	}

	f32M44 RotationMatrix(f32 angle, f32V3 axis)
	{
		// 0 vector
		if (axis.LengthSquared() == 0)
		{
			return f32M44(0);
		}
		f32V3 naxis = Normalize(axis);
		f32 ux = naxis.X();
		f32 uy = naxis.Y();
		f32 uz = naxis.Z();

		// Rotation matrix, see http://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
		// for details, see 'Mathematics for 3D Game Programming and Computer Graphics, 3rd', 4.3.1
		f32 sin = std::sin(angle);
		f32 cos = std::cos(angle);
		f32 t = 1 - cos;

		return f32M44(
			ux * ux * t + cos,
			ux * uy * t - uz * sin,
			ux * uz * t + uy * sin,
			0,

			uy * ux * t + uz * sin,
			uy * uy * t + cos,
			uy * uz * t - ux * sin,
			0,

			uz * ux * t - uy * sin,
			uz * uy * t + ux * sin,
			uz * uz * t + cos,
			0,

			0,
			0,
			0,
			1);
	}

	f32M44 RotationMatrixFromTo(f32V3 const& from, f32V3 const& to)
	{
		// 0 vector
		if (from.LengthSquared() == 0 || to.LengthSquared() == 0)
		{
			return f32M44(0);
		}
		f32V3 ufrom = Normalize(from);
		f32V3 uto = Normalize(to);

		// Rotation matrix build from two vectors, see 'Real-time Rendering, 3rd', 4.3.2, 'Rotation from One Vector to Another'
		f32 vx = ufrom.Y() * uto.Z() - ufrom.Z() * uto.Y();
		f32 vy = ufrom.Z() * uto.X() - ufrom.X() * uto.Z();
		f32 vz = ufrom.X() * uto.Y() - ufrom.Y() * uto.X();
		f32 e = ufrom.X() * uto.X() + ufrom.Y() * uto.Y() + ufrom.Z() * uto.Z();
		f32 h = 1.f / (1.f + e);

		return f32M44(
			e + h * vx * vx,
			h * vx * vy + vz,
			h * vx * vz - vy,
			0,

			h * vx * vy - vz,
			e + h * vy * vy,
			h * vy * vz + vx,
			0,

			h * vx * vz + vy,
			h * vy * vz - vx,
			e + h * vz * vz,
			0,

			0,
			0,
			0,
			1);
	}


	f32M44 LookToViewMatrix(f32V3 const& eye, f32V3 const& to, f32V3 const& up)
	{
		if (to.LengthSquared() == 0)
		{
			return f32M44(1);
		}

		if (up.LengthSquared() == 0)
		{
			return f32M44(1);
		}


		f32V3 zAxis(Normalize(to));
		f32V3 xAxis(Normalize(Cross(up, zAxis)));
		f32V3 yAxis(Cross(zAxis, xAxis));

		return f32M44(
			xAxis.X(),
			yAxis.X(),
			zAxis.X(),
			0,

			xAxis.Y(),
			yAxis.Y(),
			zAxis.Y(),
			0,

			xAxis.Z(),
			yAxis.Z(),
			zAxis.Z(),
			0,

			-Dot(xAxis, eye),
			-Dot(yAxis, eye),
			-Dot(zAxis, eye),
			1);
	}

	f32M44 FrustumProjectionMatrix(f32 fieldOfView, f32 aspectRatio, f32 near, f32 far)
	{
		f32 top = std::tan(fieldOfView * f32(0.5)) * near;
		f32 bottom = -top;
		f32 right = top * aspectRatio;
		f32 left = -right;
		return FrustumProjectionMatrix(left, right, bottom, top, near, far);
	}

	f32M44 FrustumProjectionMatrix(f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far)
	{
		// for details, see 'Real-time Rendering, 3rd', 4.6.2
		// or 'Mathematics for 3D Game Programming and Computer Graphics, 3rd', 5.5.1
		// the code here is different from the books because there +z is the view direction and z range from 0 to 1.
		f32 rl = (right - left), tb = (top - bottom), fn = (far - near);
		return f32M44(
			(near * 2) / rl,
			0,
			0,
			0,

			0,
			(near * 2) / tb,
			0,
			0,

			(right + left) / rl,
			(top + bottom) / tb,
			(far) / fn,
			1,

			0,
			0,
			-(far * near) / fn, // z map from (near, far) to (0, 1)
			0);
	};

}
