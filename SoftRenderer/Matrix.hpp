#pragma once

#include "BasicType.hpp"

#include <limits>

namespace X
{
	/*
	*	Row major matrix.
	*	Stored as [row][column].
	*/
	class f32M44
	{

	public:
		// uninitialized
		f32M44()
		{
		}
		/*
		*	Create a scalar equivalent matrix, fill principal diagonal with right.
		*/
		explicit f32M44(f32 right)
			: m00_(right), m01_(0), m02_(0), m03_(0),
			m10_(0), m11_(right), m12_(0), m13_(0),
			m20_(0), m21_(0), m22_(right), m23_(0),
			m30_(0), m31_(0), m32_(0), m33_(right)
		{
		}

		f32M44(f32M44 const& right)
			: values_(right.values_)
		{
		}

		f32M44(f32 m00, f32 m01, f32 m02, f32 m03,
			f32 m10, f32 m11, f32 m12, f32 m13,
			f32 m20, f32 m21, f32 m22, f32 m23,
			f32 m30, f32 m31, f32 m32, f32 m33)
			: m00_(m00), m01_(m01), m02_(m02), m03_(m03),
			m10_(m10), m11_(m11), m12_(m12), m13_(m13),
			m20_(m20), m21_(m21), m22_(m22), m23_(m23),
			m30_(m30), m31_(m31), m32_(m32), m33_(m33)
		{
		}

		f32 M00() const
		{
			return m00_;
		}
		f32 M01() const
		{
			return m01_;
		}
		f32 M02() const
		{
			return m02_;
		}
		f32 M03() const
		{
			return m03_;
		}

		f32 M10() const
		{
			return m10_;
		}
		f32 M11() const
		{
			return m11_;
		}
		f32 M12() const
		{
			return m12_;
		}
		f32 M13() const
		{
			return m13_;
		}

		f32 M20() const
		{
			return m20_;
		}
		f32 M21() const
		{
			return m21_;
		}
		f32 M22() const
		{
			return m22_;
		}
		f32 M23() const
		{
			return m23_;
		}

		f32 M30() const
		{
			return m30_;
		}
		f32 M31() const
		{
			return m31_;
		}
		f32 M32() const
		{
			return m32_;
		}
		f32 M33() const
		{
			return m33_;
		}

		f32 operator ()(u32 row, u32 column) const
		{
			return values_[column][row];
		}

		/*
		*	@index: row major index.
		*/
		f32 operator [](u32 index) const
		{
			return *(&values_[0][0] + index);
		}

		friend f32M44 operator +(f32M44 const& left, f32M44 const& right)
		{
			return f32M44(
				left.m00_ + right.m00_,
				left.m01_ + right.m01_,
				left.m02_ + right.m02_,
				left.m03_ + right.m03_,

				left.m10_ + right.m10_,
				left.m11_ + right.m11_,
				left.m12_ + right.m12_,
				left.m13_ + right.m13_,

				left.m20_ + right.m20_,
				left.m21_ + right.m21_,
				left.m22_ + right.m22_,
				left.m23_ + right.m23_,

				left.m30_ + right.m30_,
				left.m31_ + right.m31_,
				left.m32_ + right.m32_,
				left.m33_ + right.m33_);
		}
		friend f32M44 operator -(f32M44 const& left, f32M44 const& right)
		{
			return f32M44(
				left.m00_ - right.m00_,
				left.m01_ - right.m01_,
				left.m02_ - right.m02_,
				left.m03_ - right.m03_,

				left.m10_ - right.m10_,
				left.m11_ - right.m11_,
				left.m12_ - right.m12_,
				left.m13_ - right.m13_,

				left.m20_ - right.m20_,
				left.m21_ - right.m21_,
				left.m22_ - right.m22_,
				left.m23_ - right.m23_,

				left.m30_ - right.m30_,
				left.m31_ - right.m31_,
				left.m32_ - right.m32_,
				left.m33_ - right.m33_);
		}

		friend f32M44 operator *(f32M44 const& left, f32M44 const& right)
		{
			return f32M44(
				left.m00_ * right.m00_ + left.m01_ * right.m10_ + left.m02_ * right.m20_ + left.m03_ * right.m30_,
				left.m00_ * right.m01_ + left.m01_ * right.m11_ + left.m02_ * right.m21_ + left.m03_ * right.m31_,
				left.m00_ * right.m02_ + left.m01_ * right.m12_ + left.m02_ * right.m22_ + left.m03_ * right.m32_,
				left.m00_ * right.m03_ + left.m01_ * right.m13_ + left.m02_ * right.m23_ + left.m03_ * right.m33_,

				left.m10_ * right.m00_ + left.m11_ * right.m10_ + left.m12_ * right.m20_ + left.m13_ * right.m30_,
				left.m10_ * right.m01_ + left.m11_ * right.m11_ + left.m12_ * right.m21_ + left.m13_ * right.m31_,
				left.m10_ * right.m02_ + left.m11_ * right.m12_ + left.m12_ * right.m22_ + left.m13_ * right.m32_,
				left.m10_ * right.m03_ + left.m11_ * right.m13_ + left.m12_ * right.m23_ + left.m13_ * right.m33_,

				left.m20_ * right.m00_ + left.m21_ * right.m10_ + left.m22_ * right.m20_ + left.m23_ * right.m30_,
				left.m20_ * right.m01_ + left.m21_ * right.m11_ + left.m22_ * right.m21_ + left.m23_ * right.m31_,
				left.m20_ * right.m02_ + left.m21_ * right.m12_ + left.m22_ * right.m22_ + left.m23_ * right.m32_,
				left.m20_ * right.m03_ + left.m21_ * right.m13_ + left.m22_ * right.m23_ + left.m23_ * right.m33_,

				left.m30_ * right.m00_ + left.m31_ * right.m10_ + left.m32_ * right.m20_ + left.m33_ * right.m30_,
				left.m30_ * right.m01_ + left.m31_ * right.m11_ + left.m32_ * right.m21_ + left.m33_ * right.m31_,
				left.m30_ * right.m02_ + left.m31_ * right.m12_ + left.m32_ * right.m22_ + left.m33_ * right.m32_,
				left.m30_ * right.m03_ + left.m31_ * right.m13_ + left.m32_ * right.m23_ + left.m33_ * right.m33_
				);
		}
		friend f32M44 operator *(f32M44 const& left, f32 right)
		{
			return f32M44(
				left.m00_ * right,
				left.m01_ * right,
				left.m02_ * right,
				left.m03_ * right,

				left.m10_ * right,
				left.m11_ * right,
				left.m12_ * right,
				left.m13_ * right,

				left.m20_ * right,
				left.m21_ * right,
				left.m22_ * right,
				left.m23_ * right,

				left.m30_ * right,
				left.m31_ * right,
				left.m32_ * right,
				left.m33_ * right);
		}
		friend f32M44 operator *(f32 left, f32M44 const& right)
		{
			return f32M44(
				left * right.m00_,
				left * right.m01_,
				left * right.m02_,
				left * right.m03_,

				left * right.m10_,
				left * right.m11_,
				left * right.m12_,
				left * right.m13_,

				left * right.m20_,
				left * right.m21_,
				left * right.m22_,
				left * right.m23_,

				left * right.m30_,
				left * right.m31_,
				left * right.m32_,
				left * right.m33_);
		}

		friend f32M44 operator /(f32M44 const& left, f32 right)
		{
			return f32M44(
				left.m00_ / right,
				left.m01_ / right,
				left.m02_ / right,
				left.m03_ / right,

				left.m10_ / right,
				left.m11_ / right,
				left.m12_ / right,
				left.m13_ / right,

				left.m20_ / right,
				left.m21_ / right,
				left.m22_ / right,
				left.m23_ / right,

				left.m30_ / right,
				left.m31_ / right,
				left.m32_ / right,
				left.m33_ / right);
		}

		f32M44& operator =(f32M44 const& right)
		{
			values_ = right.values_;
			return *this;
		}

		f32M44 const& operator +() const
		{
			return *this;
		}
		f32M44 operator -() const
		{
			return f32M44(
				-m00_, -m01_, -m02_, -m03_,
				-m10_, -m11_, -m12_, -m13_,
				-m20_, -m21_, -m22_, -m23_,
				-m30_, -m31_, -m32_, -m33_);
		}

		friend bool operator ==(f32M44 const& left, f32M44 const& right)
		{
			return left.values_ == right.values_;
		}
		friend bool operator !=(f32M44 const& left, f32M44 const& right)
		{
			return left.values_ != right.values_;
		}

		f32M44 Transpose() const
		{
			return f32M44(
				m00_, m10_, m20_, m30_,
				m01_, m11_, m21_, m31_,
				m02_, m12_, m22_, m32_,
				m03_, m13_, m23_, m33_);
		}

		f32M44 Inverse() const
		{
			f32 _0011_1001 = m00_ * m11_ - m10_ * m01_,
				_0021_2001 = m00_ * m21_ - m20_ * m01_,
				_0031_3001 = m00_ * m31_ - m30_ * m01_,
				_1021_2011 = m10_ * m21_ - m20_ * m11_,
				_1031_3011 = m10_ * m31_ - m30_ * m11_,
				_2031_3021 = m20_ * m31_ - m30_ * m21_,
				_0213_1203 = m02_ * m13_ - m12_ * m03_,
				_0223_2203 = m02_ * m23_ - m22_ * m03_,
				_0233_3203 = m02_ * m33_ - m32_ * m03_,
				_1223_2213 = m12_ * m23_ - m22_ * m13_,
				_1233_3213 = m12_ * m33_ - m32_ * m13_,
				_2233_3223 = m22_ * m33_ - m32_ * m23_;

			f32 determinant = (_0011_1001 * _2233_3223 - _0021_2001 * _1233_3213 + _0031_3001 * _1223_2213 + _1021_2011 * _0233_3203 - _1031_3011 * _0223_2203 + _2031_3021 * _0213_1203);

			// non-invertible
			if (std::abs(determinant - 0) < std::numeric_limits<f32>::epsilon())
			{
				assert(false);
			}

			f32 inverseDeterminant = f32(1) / determinant;

			return f32M44(
				(m11_ * _2233_3223 - m21_ * _1233_3213 + m31_ * _1223_2213) * inverseDeterminant,
				(-m01_ * _2233_3223 + m21_ * _0233_3203 - m31_ * _0223_2203) * inverseDeterminant,
				(m01_ * _1233_3213 - m11_ * _0233_3203 + m31_ * _0213_1203) * inverseDeterminant,
				(-m01_ * _1223_2213 + m11_ * _0223_2203 - m21_ * _0213_1203) * inverseDeterminant,

				(-m10_ * _2233_3223 + m20_ * _1233_3213 - m30_ * _1223_2213) * inverseDeterminant,
				(m00_ * _2233_3223 - m20_ * _0233_3203 + m30_ * _0223_2203) * inverseDeterminant,
				(-m00_ * _1233_3213 + m10_ * _0233_3203 - m30_ * _0213_1203) * inverseDeterminant,
				(m00_ * _1223_2213 - m10_ * _0223_2203 + m20_ * _0213_1203) * inverseDeterminant,

				(m13_ * _2031_3021 - m23_ * _1031_3011 + m33_ * _1021_2011) * inverseDeterminant,
				(-m03_ * _2031_3021 + m23_ * _0031_3001 - m33_ * _0021_2001) * inverseDeterminant,
				(m03_ * _1031_3011 - m13_ * _0031_3001 + m33_ * _0011_1001) * inverseDeterminant,
				(-m03_ * _1021_2011 + m13_ * _0021_2001 - m23_ * _0011_1001) * inverseDeterminant,

				(-m12_ * _2031_3021 + m22_ * _1031_3011 - m32_ * _1021_2011) * inverseDeterminant,
				(m02_ * _2031_3021 - m22_ * _0031_3001 + m32_ * _0021_2001) * inverseDeterminant,
				(-m02_ * _1031_3011 + m12_ * _0031_3001 - m32_ * _0011_1001) * inverseDeterminant,
				(m02_ * _1021_2011 - m12_ * _0021_2001 + m22_ * _0011_1001) * inverseDeterminant);
		}

		f32 Determinant() const
		{
			f32 _0011_1001 = m00_ * m11_ - m10_ * m01_,
				_0021_2001 = m00_ * m21_ - m20_ * m01_,
				_0031_3001 = m00_ * m31_ - m30_ * m01_,
				_1021_2011 = m10_ * m21_ - m20_ * m11_,
				_1031_3011 = m10_ * m31_ - m30_ * m11_,
				_2031_3021 = m20_ * m31_ - m30_ * m21_,
				_0213_1203 = m02_ * m13_ - m12_ * m03_,
				_0223_2203 = m02_ * m23_ - m22_ * m03_,
				_0233_3203 = m02_ * m33_ - m32_ * m03_,
				_1223_2213 = m12_ * m23_ - m22_ * m13_,
				_1233_3213 = m12_ * m33_ - m32_ * m13_,
				_2233_3223 = m22_ * m33_ - m32_ * m23_;

			f32 determinant = (_0011_1001 * _2233_3223 - _0021_2001 * _1233_3213 + _0031_3001 * _1223_2213 + _1021_2011 * _0233_3203 - _1031_3011 * _0223_2203 + _2031_3021 * _0213_1203);
			return determinant;
		}

		f32 const* GetArray() const
		{
			return &values_[0][0];
		}

	private:
		union
		{
			std::array<std::array<f32, 4>, 4> values_;
			struct
			{
				f32 m00_, m01_, m02_, m03_;
				f32 m10_, m11_, m12_, m13_;
				f32 m20_, m21_, m22_, m23_;
				f32 m30_, m31_, m32_, m33_;
			};
		};
	};

}


