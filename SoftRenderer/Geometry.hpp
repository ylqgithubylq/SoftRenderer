#pragma once
#include "Math.hpp"

namespace X
{

	class Sphere
	{
	public:
		Sphere(f32V3 const& position, f32 radius)
			: position_(position), radius_(radius)
		{
		}

		f32V3 const& GetPosition() const
		{
			return position_;
		}

		f32 GetRadius() const
		{
			return radius_;
		}

	private:
		f32V3 position_;
		f32 radius_;
	};

	/*
	*	normal.x * x + normal.y * y + normal.z * z + distance_
	*/
	class Plane
	{
	public:
		Plane()
		{
		}

		Plane(f32V3 const& normal, f32 d)
			: normal_(normal), distance_(d)
		{
		}
		Plane(f32V3 const& normal, f32V3 const& point)
			: normal_(normal), distance_(-Dot(normal, point))
		{
		}

		f32 SignedDistance(f32V3 const& position) const
		{
			return Dot(position, normal_) + distance_;
		}

		f32V3 const& GetNormal() const
		{
			return normal_;
		}

		f32 GetDistance() const
		{
			return distance_;
		}

	private:
		f32V3 normal_;
		f32 distance_;
	};


	class Frustum
	{
	public:
		enum class Face
		{
			Right,
			Left,
			Top,
			Bottom,
			Near,
			Far,
		};

		Frustum(f32 fieldOfView, f32 aspectRatio, f32 near, f32 far)
		{
			f32 top = std::tan(fieldOfView * f32(0.5)) * near;
			f32 bottom = -top;
			f32 right = top * aspectRatio;
			f32 left = -right;
			faces_[static_cast<u32>(Face::Right)] = Plane(Normalize(f32V3(1, 0, -right)), 0);
			faces_[static_cast<u32>(Face::Left)] = Plane(Normalize(f32V3(-1, 0, left)), 0);
			faces_[static_cast<u32>(Face::Top)] = Plane(Normalize(f32V3(0, 1, -top)), 0);
			faces_[static_cast<u32>(Face::Bottom)] = Plane(Normalize(f32V3(0, -1, bottom)), 0);
			faces_[static_cast<u32>(Face::Near)] = Plane(f32V3(0, 0, -1), near);
			faces_[static_cast<u32>(Face::Far)] = Plane(f32V3(0, 0, 1), -far);
		}

		Frustum(std::array<Plane, 6> const& planes)
			: faces_(planes)
		{
		}

		std::array<Plane, 6> const& GetFaces() const
		{
			return faces_;
		}

	private:
		std::array<Plane, 6> faces_; // normals point outwards
	};

	class BoundingBox
	{
	public:
		BoundingBox(f32V3 const& center, f32V3 const& halfExtend)
			: center_(center), halfExtend_(halfExtend)
		{
		}

		f32V3 const& GetPosition() const
		{
			return center_;
		}

		f32V3 const& GetHalfExtend() const
		{
			return halfExtend_;
		}

	private:
		f32V3 center_;
		f32V3 halfExtend_;
	};

	class RotatedBoundingBox
	{
	public:
		RotatedBoundingBox(f32V3 const& center, f32V3 const& halfExtend, f32M44 const& rotation)
			: center_(center), halfExtend_(halfExtend)
		{
			r_ = TransformDirection(f32V3(halfExtend.X(), 0, 0), rotation);
			s_ = TransformDirection(f32V3(0, halfExtend.Y(), 0), rotation);
			t_ = TransformDirection(f32V3(0, 0, halfExtend.Z()), rotation);
		}

		f32V3 const& GetPosition() const
		{
			return center_;
		}

		f32V3 const& GetHalfExtend() const
		{
			return halfExtend_;
		}

		f32V3 const& GetR() const
		{
			return r_;
		}
		f32V3 const& GetS() const
		{
			return s_;
		}
		f32V3 const& GetT() const
		{
			return t_;
		}

	private:
		f32V3 center_;
		f32V3 halfExtend_;
		f32V3 r_;
		f32V3 s_;
		f32V3 t_;
	};
}


