#pragma once
#include "Component.hpp"
#include "Common.hpp"

namespace X
{
	class Light
		: public Component
	{
	public:
		Light();
		virtual ~Light() override;

		virtual ComponentType GetComponentType() const override
		{
			return ComponentType::Light;
		}

	};

	class AmbientLight
		: public Light
	{
	public:
		AmbientLight(f32V3 const& intensity);
		virtual ~AmbientLight() override;

		f32V3 GetLightIntensity()
		{
			return intensity_;
		}

	private:
		f32V3 intensity_;
	};

	class DirectionalLight
		: public Light
	{
	public:
		DirectionalLight(f32V3 const& intensity);
		virtual ~DirectionalLight() override;

		f32V3 GetLightIntensity()
		{
			return intensity_;
		}

	private:
		f32V3 intensity_;
	};

	class PointLight
		: public Light
	{
	public:
		PointLight(f32V3 const& intensity, f32 radius, f32 scale = 1.f);
		virtual ~PointLight() override;

		f32V3 GetLightIntensity(f32V3 const& lightPosition, f32V3 const& objectPosition);

		f32V3 GetLightDirection(f32V3 const& lightPosition, f32V3 const& objectPosition)
		{
			return Normalize(lightPosition - objectPosition);

		}

		f32 GetRadius() const
		{
			return radius_;
		}

	private:
		f32V3 intensity_;
		f32 radius_;
		f32 inverseScaleSquare_;
	};
}


