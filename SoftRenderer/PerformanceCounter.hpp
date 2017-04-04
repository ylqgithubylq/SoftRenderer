#pragma once
#include "Common.hpp"

namespace X
{
	class PerformanceCounter
	{
	public:
		enum class Term
		{
			All,
			Render,
			
			DeferredGeometryPass,
			DeferredShadingPass,

			ForwardPreZPass,
			ForwardRenderPass,

			ForwardTotalRasterize, // accurate only under single thread

			DeferredLightTransform,
			DeferredVertex, // accurate only under single thread
			DeferredRasterizeAndPixel, // accurate only under single thread

			TiledFrustumCulling, // accurate only under single thread
			TiledShading, // accurate only under single thread

			TermCount,
		};

	public:
		PerformanceCounter(Context& context);
		~PerformanceCounter();

		void Begin(Term term);
		void End(Term term);

		f32 Get(Term term);
		f64 GetStartTime(Term term);

		void Clear(Term term);
		void ClearAll();

	private:
		struct Impl;
		std::unique_ptr<Impl> impl_;
	};
}


