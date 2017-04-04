#include "Header.hpp"
#include "Renderer.hpp"
#include "Pipeline.hpp"
#include "Context.hpp"
#include "Scene.hpp"
#include "MainWindow.hpp"

namespace X
{
	Renderer::Renderer(Context& context)
		: context_(context)
	{
		colorBuffer_ = std::make_unique<ConcreteTexture2D<f32V3>>(context_.GetMainWindow().GetClientRegionSize());
	}


	Renderer::~Renderer()
	{
	}

	std::unique_ptr<Pipeline> Renderer::SetPipeline(std::unique_ptr<Pipeline> pipeline)
	{
		std::swap(pipeline, pipeline_);
		return pipeline;
	}

	void Renderer::RenderAFrame(f64 current, f32 delta)
	{
		assert(pipeline_ != nullptr);
		pipeline_->RenderScene(current, delta);
	}

}

