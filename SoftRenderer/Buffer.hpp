#pragma once
#include "Common.hpp"
#include "Primitive.hpp"

namespace X
{
	class Buffer
		: Noncopyable
	{
	public:
		Buffer();
		virtual ~Buffer();
	};


	class IndexBuffer
		: public Buffer
	{
	public:
		IndexBuffer(std::vector<u16> indices)
			: indices_(std::move(indices))
		{
		}
		virtual ~IndexBuffer() override
		{
		}

		void SetData(std::vector<u16> indices)
		{
			indices_ = std::move(indices);
		}
		std::vector<u16> const& GetData() const
		{
			return indices_;
		}

	private:
		std::vector<u16> indices_;
	};



	class VertexBuffer
		: public Buffer
	{
	public:
		VertexBuffer(std::vector<Vertex> vertices)
			: vertices_(std::move(vertices))
		{
		}
		virtual ~VertexBuffer() override
		{
		}

		void SetData(std::vector<Vertex> vertices)
		{
			vertices_ = std::move(vertices);
		}
		std::vector<Vertex> const& GetData() const
		{
			return vertices_;
		}

	private:
		std::vector<Vertex> vertices_;
	};
}


