#include "Header.hpp"
#include "GeometryMath.hpp"

namespace X
{


	bool Intersect(Plane const& plane, RotatedBoundingBox const& box)
	{
		// for details, see 'Mathematics for 3D Game Programming and Computer Graphics, 3rd', 8.2.4
		f32V3 const& normal = plane.GetNormal();		
		f32 effectiveRadius = std::abs(Dot(box.GetR(), normal)) + std::abs(Dot(box.GetS(), normal)) + std::abs(Dot(box.GetT(), normal));
		f32 distance = plane.SignedDistance(box.GetPosition());
		return (distance - effectiveRadius <= 0);
	}


	bool IntersectRough(RotatedBoundingBox const& box, Frustum const& frustum)
	{
		std::array<Plane, 6> const& faces = frustum.GetFaces();
		for (Plane const& p : faces)
		{
			if (!Intersect(p, box))
			{
				return false;
			}
		}
		return true;
	}

	bool IntersectRough(Sphere const& sphere, Frustum const& frustum)
	{
		std::array<Plane, 6> const& faces = frustum.GetFaces();
		for (Plane const& p : faces)
		{
			if (!Intersect(p, sphere))
			{
				return false;
			}
		}
		return true;
	}

	RotatedBoundingBox Transform(BoundingBox const& box, f32M44 const& matrix)
	{
		f32V3 position = Transform(box.GetPosition(), matrix);
		return RotatedBoundingBox(position, box.GetHalfExtend(), matrix);
	}

}