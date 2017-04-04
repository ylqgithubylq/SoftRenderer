#pragma once
#include "Common.hpp"
#include "Primitive.hpp"
#include "PipelineDetail.hpp"
#include "DefferredPipeline.hpp"
#include "Texture2D.hpp"

namespace X
{
	class Rasterizer
		: Noncopyable
	{
	};

	class LineRasterizer
		: public Rasterizer
	{
	public:
		template <typename ContinuationT>
		void Rasterize(Size<u32, 2> resolution, ConcreteTexture2D<f32>& depthBuffer,
			ContinuationT const& fragmentContinuation, Triangle const& triangle);
	};

	class FillRasterizer
		: public Rasterizer
	{
	public:
		template <typename ContinuationT>
		void Rasterize(Size<u32, 2> resolution, ConcreteTexture2D<f32>& depthBuffer,
			ContinuationT const& fragmentContinuation, Triangle const& triangle);

	};
}

#include "RasterizerDetail.hpp"

