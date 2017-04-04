#pragma once
#include "Common.hpp"

namespace X
{
	class Pipeline
		: Noncopyable
	{
	public:
		Pipeline(Renderer& renderer, Size<u32, 2> const& bufferSize);
		virtual ~Pipeline();

		virtual void RenderScene(f64 current, f32 delta) = 0;

		Size<u32, 2> GetBufferSize() const
		{
			return bufferSize_;
		}


		Renderer& GetRenderer() const
		{
			return renderer_;
		}

	private:
		Renderer& renderer_;

		Size<u32, 2> bufferSize_;
	};
}


