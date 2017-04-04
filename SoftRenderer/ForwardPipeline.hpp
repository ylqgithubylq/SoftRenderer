#pragma once
#include "Pipeline.hpp"

namespace X
{
	class ForwardPipeline
		: public Pipeline
	{
	public:
		ForwardPipeline(Renderer& renderer, Size<u32, 2> const& bufferSize);
		virtual ~ForwardPipeline() override;

		virtual void RenderScene(f64 current, f32 delta) override;

	private:

		struct Impl;
		std::unique_ptr<Impl> impl_;

	};
}


