#pragma once

#include "Common.hpp"
#include "Texture2D.hpp"

namespace X
{
	class Renderer
		: Noncopyable
	{
	public:

		Renderer(Context& context);
		~Renderer();
		
		/*
		*	@return: old one
		*/
		std::unique_ptr<Pipeline> SetPipeline(std::unique_ptr<Pipeline> pipeline);

		Pipeline* GetPipeline() const
		{
			return pipeline_.get();
		}

		void RenderAFrame(f64 current, f32 delta);

		Context& GetContext() const
		{
			return context_;
		}

		ConcreteTexture2D<f32V3>& GetColorBuffer()
		{
			return *colorBuffer_;
		}

	private:
		Context& context_;
		std::unique_ptr<ConcreteTexture2D<f32V3>> colorBuffer_;
		std::unique_ptr<Pipeline> pipeline_;
	};
}


