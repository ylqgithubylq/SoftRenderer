#pragma once
#include "Math.hpp"
#include "Geometry.hpp"

namespace X
{
	/*
	*	It always considered as intersected when sphere is at negative half plane.
	*/
	inline bool Intersect(Plane const& plane, Sphere const& sphere)
	{
		return plane.SignedDistance(sphere.GetPosition()) <= sphere.GetRadius();
	}
	/*
	*	It always considered as intersected when sphere is at negative half plane.
	*/
	inline bool Intersect(Sphere const& sphere, Plane const& plane)
	{
		return Intersect(plane, sphere);
	}
	/*
	*	It always considered as intersected when box is at negative half plane.
	*/
	bool Intersect(Plane const& plane, RotatedBoundingBox const& box);
	/*
	*	It always considered as intersected when box is at negative half plane.
	*/
	inline bool Intersect(RotatedBoundingBox const& box, Plane const& plane)
	{
		return Intersect(plane, box);
	}

	bool IntersectRough(RotatedBoundingBox const& box, Frustum const& frustum);
	inline bool IntersectRough(Frustum const& frustum, RotatedBoundingBox const& box)
	{
		return IntersectRough(box, frustum);
	}

	bool IntersectRough(Sphere const& sphere, Frustum const& frustum);
	inline bool IntersectRough(Frustum const& frustum, Sphere const& sphere)
	{
		return IntersectRough(sphere, frustum);
	}

	RotatedBoundingBox Transform(BoundingBox const& box, f32M44 const& matrix);
}