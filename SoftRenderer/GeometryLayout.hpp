#pragma once
#include "Common.hpp"
#include "Buffer.hpp"

namespace X
{
	class GeometryLayout
	{
	public:
		GeometryLayout(std::shared_ptr<VertexBuffer> vertexBuffer, std::shared_ptr<IndexBuffer> indexBuffer)
			: vertexBuffer_(std::move(vertexBuffer)), indexBuffer_(std::move(indexBuffer))
		{
		}

		std::shared_ptr<VertexBuffer> const& GetVertexBuffer() const
		{
			return vertexBuffer_;
		}

		std::shared_ptr<IndexBuffer> const& GetIndexBuffer() const
		{
			return indexBuffer_;
		}

	private:
		std::shared_ptr<VertexBuffer> vertexBuffer_;
		std::shared_ptr<IndexBuffer> indexBuffer_;
	};
}


