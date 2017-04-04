#pragma once
#include "Common.hpp"
#include "Texture2D.hpp"
#include "Sampler.hpp"
namespace X
{

	class SurfaceShader
		: Noncopyable
	{
	public:

		virtual f32V3 Shading(f32V3 const& diffuse, f32V3 const& normal, f32V3 const& half, f32V3 const& viewDirection, f32V3 const& lightDirection) = 0;
	};

	class Material
		: Noncopyable
	{
	public:
		enum class RasterizeMode
		{
			Line,
			Fill,
		};

	public:
		Material();
		~Material();

		void SetRasterizeMode(RasterizeMode rasterizeMode)
		{
			rasterizeMode_ = rasterizeMode;
		}
		RasterizeMode GetRasterizeMode() const
		{
			return rasterizeMode_;
		}

		void SetDiffuseTexture(std::shared_ptr<ConcreteTexture2D<f32V3>> diffuseTexture)
		{
			diffuseTexture_ = std::move(diffuseTexture);
		}
		std::shared_ptr<ConcreteTexture2D<f32V3>> const& GetDiffuseTexture() const
		{
			return diffuseTexture_;
		}

		void SetDiffuseSampler(std::shared_ptr<PointSampler<Sampler::RepeatAddresser>> diffuseSampler)
		{
			diffuseSampler_ = std::move(diffuseSampler);
		}
		std::shared_ptr<PointSampler<Sampler::RepeatAddresser>> const& GetDiffuseSampler() const
		{
			return diffuseSampler_;
		}

		void SetSurfaceShader(std::shared_ptr<SurfaceShader> surfaceShader)
		{
			surfaceShader_ = std::move(surfaceShader);
		}
		std::shared_ptr<SurfaceShader> const& GetSurfaceShader() const
		{
			return surfaceShader_;
		}

	private:
		RasterizeMode rasterizeMode_;
		std::shared_ptr<ConcreteTexture2D<f32V3>> diffuseTexture_;
		std::shared_ptr<PointSampler<Sampler::RepeatAddresser>> diffuseSampler_;
		std::shared_ptr<SurfaceShader> surfaceShader_;
	};
}


