#pragma once

#include "BasicType.hpp"

namespace X
{
	class f32V2
	{
	public:
		// uninitialized
		f32V2()
		{
		}
		f32V2(f32 x, f32 y)
		{
			x_ = x;
			y_ = y;
		}

		f32V2(f32V2 const& right)
		{
			values_ = right.values_;
		}

		f32V2& operator =(f32V2 const& right)
		{
			values_ = right.values_;
			return *this;
		}

		f32 X() const
		{
			return x_;
		}
		f32 Y() const
		{
			return y_;
		}


		f32 operator [](u32 index) const
		{
			assert(index < 2);
			return values_[index];
		}

		f32V2& operator +=(f32V2 const& right)
		{
			x_ += right.x_; y_ += right.y_;
			return *this;
		}
		f32V2& operator *=(f32V2 const& right)
		{
			x_ *= right.x_; y_ *= right.y_;
			return *this;
		}
		f32V2& operator *=(f32 right)
		{
			x_ *= right; y_ *= right;
			return *this;
		}

		friend f32V2 operator +(f32V2 const& left, f32V2 const& right)
		{
			return f32V2(left.x_ + right.x_, left.y_ + right.y_);
		}

		friend f32V2 operator -(f32V2 const& left, f32V2 const& right)
		{
			return f32V2(left.x_ - right.x_, left.y_ - right.y_);
		}

		friend f32V2 operator *(f32V2 const& left, f32V2 const& right)
		{
			return f32V2(left.x_ * right.x_, left.y_ * right.y_);
		}

		friend f32V2 operator *(f32V2 const& left, f32 right)
		{
			return f32V2(left.x_ *right, left.y_ * right);

		}
		friend f32V2 operator *(f32 left, f32V2 const& right)
		{
			return f32V2(left * right.x_, left * right.y_);
		}

		friend f32V2 operator /(f32V2 const& left, f32V2 const& right)
		{
			return f32V2(left.x_ / right.x_, left.y_ / right.y_);
		}

		friend f32V2 operator /(f32V2 const& left, f32 right)
		{
			return f32V2(left.x_ / right, left.y_ / right);
		}

		f32V2 const& operator +() const
		{
			return *this;
		}
		f32V2 operator -() const
		{
			return f32V2(-this->x_, -this->y_);
		}

		friend bool operator ==(f32V2 const& left, f32V2 const& right)
		{
			return left.values_ == right.values_;
		}

		friend bool operator !=(f32V2 const& left, f32V2 const& right)
		{
			return left.values_ != right.values_;
		}

		f32 Length() const
		{
			return std::sqrt(LengthSquared());
		}

		f32 LengthSquared() const
		{
			return Dot(*this, *this);
		}

		friend f32 Dot(f32V2 const& left, f32V2 const& right)
		{
			return left.x_ * right.x_ + left.y_ * right.y_;
		}


		f32 const* GetArray() const
		{
			return &values_[0];
		}

	private:
		union
		{
			std::array<f32, 2> values_;
			struct
			{
				f32 x_;
				f32 y_;
			};
		};
	};

	inline f32V2 Normalize(f32V2 const& vector)
	{
		return vector * (1 / vector.Length());
	}


	class f32V3
	{
	public:
		// uninitialized
		f32V3()
		{
		}
		f32V3(f32 x, f32 y, f32 z)
		{
			x_ = x;
			y_ = y;
			z_ = z;
		}

		f32V3(f32V3 const& right)
		{
			values_ = right.values_;
		}

		f32V3& operator =(f32V3 const& right)
		{
			values_ = right.values_;
			return *this;
		}

		f32 X() const
		{
			return x_;
		}
		f32 Y() const
		{
			return y_;
		}
		f32 Z() const
		{
			return z_;
		}

		f32 operator [](u32 index) const
		{
			assert(index < 3);
			return values_[index];
		}


		f32V3& operator +=(f32V3 const& right)
		{
			x_ += right.x_; y_ += right.y_; z_ += right.z_;
			return *this;
		}
		f32V3& operator *=(f32V3 const& right)
		{
			x_ *= right.x_; y_ *= right.y_; z_ *= right.z_;
			return *this;
		}
		f32V3& operator *=(f32 right)
		{
			x_ *= right; y_ *= right; z_ *= right;
			return *this;
		}

		friend f32V3 operator +(f32V3 const& left, f32V3 const& right)
		{
			return f32V3(left.x_ + right.x_, left.y_ + right.y_, left.z_ + right.z_);
		}

		friend f32V3 operator -(f32V3 const& left, f32V3 const& right)
		{
			return f32V3(left.x_ - right.x_, left.y_ - right.y_, left.z_ - right.z_);

		}

		friend f32V3 operator *(f32V3 const& left, f32V3 const& right)
		{
			return f32V3(left.x_ * right.x_, left.y_ * right.y_, left.z_ * right.z_);
		}

		friend f32V3 operator *(f32V3 const& left, f32 right)
		{
			return f32V3(left.x_ *right, left.y_ * right, left.z_ * right);

		}
		friend f32V3 operator *(f32 left, f32V3 const& right)
		{
			return f32V3(left * right.x_, left * right.y_, left * right.z_);
		}

		friend f32V3 operator /(f32V3 const& left, f32V3 const& right)
		{
			return f32V3(left.x_ / right.x_, left.y_ / right.y_, left.z_ / right.z_);
		}

		friend f32V3 operator /(f32V3 const& left, f32 right)
		{
			return f32V3(left.x_ / right, left.y_ / right, left.z_ / right);
		}

		f32V3 const& operator +() const
		{
			return *this;
		}
		f32V3 operator -() const
		{
			return f32V3(-this->x_, -this->y_, -this->z_);
		}

		friend bool operator ==(f32V3 const& left, f32V3 const& right)
		{
			return left.values_ == right.values_;
		}

		friend bool operator !=(f32V3 const& left, f32V3 const& right)
		{
			return left.values_ != right.values_;
		}

		f32 Length() const
		{
			return std::sqrt(LengthSquared());
		}

		f32 LengthSquared() const
		{
			return Dot(*this, *this);
		}

		friend f32 Dot(f32V3 const& left, f32V3 const& right)
		{
			return left.x_ * right.x_ + left.y_ * right.y_ + left.z_ * right.z_;
		}

		friend f32V3 Cross(f32V3 const& left, f32V3 const& right)
		{
			return f32V3(
				left.y_ * right.z_ - left.z_ * right.y_,
				left.z_ * right.x_ - left.x_ * right.z_,
				left.x_ * right.y_ - left.y_ * right.x_);
		}


		f32 const* GetArray() const
		{
			return &values_[0];
		}

	private:
		union
		{
			std::array<f32, 3> values_;
			struct
			{
				f32 x_;
				f32 y_;
				f32 z_;
			};
		};
	};

	inline f32V3 Normalize(f32V3 const& vector)
	{
		return vector * (1 / vector.Length());
	}


	class f32V4
	{
	public:
		// uninitialized
		f32V4()
		{
		}
		f32V4(f32 x, f32 y, f32 z, f32 w)
		{
			values_[0] = x;
			values_[1] = y;
			values_[2] = z;
			values_[3] = w;
		}

		f32V4(f32V4 const& right)
		{
			values_ = right.values_;
		}

		f32V4& operator =(f32V4 const& right)
		{
			values_ = right.values_;
			return *this;
		}

		f32 X() const
		{
			return x_;
		}
		f32 Y() const
		{
			return y_;
		}
		f32 Z() const
		{
			return z_;
		}
		f32 W() const
		{
			return w_;
		}

		f32 operator [](u32 index) const
		{
			assert(index < 4);
			return values_[index];
		}

		friend f32V4 operator +(f32V4 const& left, f32V4 const& right)
		{
			return f32V4(left.x_ + right.x_, left.y_ + right.y_, left.z_ + right.z_, left.w_ + right.w_);
		}

		friend f32V4 operator -(f32V4 const& left, f32V4 const& right)
		{
			return f32V4(left.x_ - right.x_, left.y_ - right.y_, left.z_ - right.z_, left.w_ - right.w_);

		}

		friend f32V4 operator *(f32V4 const& left, f32V4 const& right)
		{
			return f32V4(left.x_ * right.x_, left.y_ * right.y_, left.z_ * right.z_, left.w_ * right.w_);
		}

		friend f32V4 operator *(f32V4 const& left, f32 right)
		{
			return f32V4(left.x_ *right, left.y_ * right, left.z_ * right, left.w_ * right);

		}
		friend f32V4 operator *(f32 left, f32V4 const& right)
		{
			return f32V4(left * right.x_, left * right.y_, left * right.z_, left * right.w_);
		}

		friend f32V4 operator /(f32V4 const& left, f32V4 const& right)
		{
			return f32V4(left.x_ / right.x_, left.y_ / right.y_, left.z_ / right.z_, left.w_ / right.w_);
		}

		friend f32V4 operator /(f32V4 const& left, f32 right)
		{
			return f32V4(left.x_ / right, left.y_ / right, left.z_ / right, left.w_ / right);
		}

		f32V4 const& operator +() const
		{
			return *this;
		}
		f32V4 operator -() const
		{
			return f32V4(-this->x_, -this->y_, -this->z_, -this->w_);
		}

		friend bool operator ==(f32V4 const& left, f32V4 const& right)
		{
			return left.values_ == right.values_;
		}

		friend bool operator !=(f32V4 const& left, f32V4 const& right)
		{
			return left.values_ != right.values_;
		}

		f32 Length() const
		{
			return std::sqrt(LengthSquared());
		}

		f32 LengthSquared() const
		{
			return Dot(*this, *this);
		}

		friend f32 Dot(f32V4 const& left, f32V4 const& right)
		{
			return left.x_ * right.x_ + left.y_ * right.y_ + left.z_ * right.z_ + left.w_ * right.w_;
		}

		f32 const* GetArray() const
		{
			return &values_[0];
		}

	private:
		union
		{
			std::array<f32, 4> values_;
			struct
			{
				f32 x_;
				f32 y_;
				f32 z_;
				f32 w_;
			};
		};
	};

	inline f32V4 Normalize(f32V4 const& vector)
	{
		return vector * (1 / vector.Length());
	}
}
