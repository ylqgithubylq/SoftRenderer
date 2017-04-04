#include "Header.hpp"
#include "GeometryUtility.hpp"
#include "Buffer.hpp"
#include "Primitive.hpp"
#include "GeometryLayout.hpp"

namespace X
{

	std::shared_ptr<GeometryLayout> MakeCube(f32 halfSize)
	{
		std::vector<Vertex> vertices;

		vertices.push_back(Vertex(f32V3(-halfSize, -halfSize, -halfSize), f32V3(0, 0, -1), f32V2(0, 0)));
		vertices.push_back(Vertex(f32V3(halfSize, -halfSize, -halfSize), f32V3(0, 0, -1), f32V2(1, 0)));
		vertices.push_back(Vertex(f32V3(-halfSize, halfSize, -halfSize), f32V3(0, 0, -1), f32V2(0, 1)));
		vertices.push_back(Vertex(f32V3(halfSize, halfSize, -halfSize), f32V3(0, 0, -1), f32V2(1, 1)));

		vertices.push_back(Vertex(f32V3(halfSize, -halfSize, -halfSize), f32V3(1, 0, 0), f32V2(0, 0)));
		vertices.push_back(Vertex(f32V3(halfSize, -halfSize, halfSize), f32V3(1, 0, 0), f32V2(1, 0)));
		vertices.push_back(Vertex(f32V3(halfSize, halfSize, -halfSize), f32V3(1, 0, 0), f32V2(0, 1)));
		vertices.push_back(Vertex(f32V3(halfSize, halfSize, halfSize), f32V3(1, 0, 0), f32V2(1, 1)));

		vertices.push_back(Vertex(f32V3(halfSize, -halfSize, halfSize), f32V3(0, 0, 1), f32V2(0, 0)));
		vertices.push_back(Vertex(f32V3(-halfSize, -halfSize, halfSize), f32V3(0, 0, 1), f32V2(1, 0)));
		vertices.push_back(Vertex(f32V3(halfSize, halfSize, halfSize), f32V3(0, 0, 1), f32V2(0, 1)));
		vertices.push_back(Vertex(f32V3(-halfSize, halfSize, halfSize), f32V3(0, 0, 1), f32V2(1, 1)));

		vertices.push_back(Vertex(f32V3(-halfSize, -halfSize, halfSize), f32V3(-1, 0, 0), f32V2(0, 0)));
		vertices.push_back(Vertex(f32V3(-halfSize, -halfSize, -halfSize), f32V3(-1, 0, 0), f32V2(1, 0)));
		vertices.push_back(Vertex(f32V3(-halfSize, halfSize, halfSize), f32V3(-1, 0, 0), f32V2(0, 1)));
		vertices.push_back(Vertex(f32V3(-halfSize, halfSize, -halfSize), f32V3(-1, 0, 0), f32V2(1, 1)));

		vertices.push_back(Vertex(f32V3(-halfSize, halfSize, -halfSize), f32V3(0, 1, 0), f32V2(0, 0)));
		vertices.push_back(Vertex(f32V3(halfSize, halfSize, -halfSize), f32V3(0, 1, 0), f32V2(1, 0)));
		vertices.push_back(Vertex(f32V3(-halfSize, halfSize, halfSize), f32V3(0, 1, 0), f32V2(0, 1)));
		vertices.push_back(Vertex(f32V3(halfSize, halfSize, halfSize), f32V3(0, 1, 0), f32V2(1, 1)));

		vertices.push_back(Vertex(f32V3(-halfSize, -halfSize, halfSize), f32V3(0, -1, 0), f32V2(0, 0)));
		vertices.push_back(Vertex(f32V3(halfSize, -halfSize, halfSize), f32V3(0, -1, 0), f32V2(1, 0)));
		vertices.push_back(Vertex(f32V3(-halfSize, -halfSize, -halfSize), f32V3(0, -1, 0), f32V2(0, 1)));
		vertices.push_back(Vertex(f32V3(halfSize, -halfSize, -halfSize), f32V3(0, -1, 0), f32V2(1, 1)));

		std::vector<u16> indices;
		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);
		indices.push_back(2);
		indices.push_back(1);
		indices.push_back(3);

		indices.push_back(0 + 4);
		indices.push_back(1 + 4);
		indices.push_back(2 + 4);
		indices.push_back(2 + 4);
		indices.push_back(1 + 4);
		indices.push_back(3 + 4);

		indices.push_back(0 + 8);
		indices.push_back(1 + 8);
		indices.push_back(2 + 8);
		indices.push_back(2 + 8);
		indices.push_back(1 + 8);
		indices.push_back(3 + 8);

		indices.push_back(0 + 12);
		indices.push_back(1 + 12);
		indices.push_back(2 + 12);
		indices.push_back(2 + 12);
		indices.push_back(1 + 12);
		indices.push_back(3 + 12);

		indices.push_back(0 + 16);
		indices.push_back(1 + 16);
		indices.push_back(2 + 16);
		indices.push_back(2 + 16);
		indices.push_back(1 + 16);
		indices.push_back(3 + 16);

		indices.push_back(0 + 20);
		indices.push_back(1 + 20);
		indices.push_back(2 + 20);
		indices.push_back(2 + 20);
		indices.push_back(1 + 20);
		indices.push_back(3 + 20);

		return std::make_shared<GeometryLayout>(std::make_shared<VertexBuffer>(std::move(vertices)), std::make_shared<IndexBuffer>(std::move(indices)));

	}

	std::shared_ptr<GeometryLayout> MakeCone(f32 radius, f32 height, u32 radiusSplittingCount)
	{
		return nullptr;
	}

}

