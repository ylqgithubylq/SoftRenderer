#pragma once
#include "Common.hpp"
#include "Primitive.hpp"
#include "Light.hpp"
namespace X
{
	struct AttributeInputPackage
	{
		Vertex const& vertex;

		AttributeInputPackage(Vertex const& vertex)
			: vertex(vertex)
		{
		}
	};

	struct AttributeOutputPackage
	{
		Vertex vertex;
		f32V4 position;
		AttributeOutputPackage()
		{
		}
		AttributeOutputPackage(Vertex const& vertex, f32V4 const& position)
			: vertex(vertex), position(position)
		{
		}
	};


	struct Triangle
	{
		AttributeOutputPackage& v0;
		AttributeOutputPackage& v1;
		AttributeOutputPackage& v2;
		Triangle(AttributeOutputPackage& v0, AttributeOutputPackage& v1, AttributeOutputPackage& v2)
			: v0(v0), v1(v1), v2(v2)
		{
		}
	};
}

