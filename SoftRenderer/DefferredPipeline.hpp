#pragma once
#include "Pipeline.hpp"

namespace X
{
	/*
	*	Deferred texturing pipeline.
	*/
	class DefferredPipeline
		: public Pipeline
	{
	public:
		DefferredPipeline(Renderer& renderer, Size<u32, 2> const& bufferSize);
		virtual ~DefferredPipeline() override;

		virtual void RenderScene(f64 current, f32 delta) override;


	private:
		struct Impl;
		std::unique_ptr<Impl> impl_;
	};
}


