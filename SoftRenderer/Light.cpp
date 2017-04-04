#include "Header.hpp"
#include "Light.hpp"
#include "Entity.hpp"
namespace X
{
	Light::Light()
	{
	}


	Light::~Light()
	{
	}



	AmbientLight::AmbientLight(f32V3 const& intensity)
		: intensity_(intensity)
	{
	}

	AmbientLight::~AmbientLight()
	{
	}


	DirectionalLight::DirectionalLight(f32V3 const& intensity)
		: intensity_(intensity)
	{
	}

	DirectionalLight::~DirectionalLight()
	{
	}



	PointLight::PointLight(f32V3 const& intensity, f32 radius, f32 scale)
		: intensity_(intensity), radius_(radius), inverseScaleSquare_(1 / Square(scale))
	{
	}

	PointLight::~PointLight()
	{
	}

	f32V3 PointLight::GetLightIntensity(f32V3 const& lightPosition, f32V3 const& objectPosition)
	{
		f32V3 l = lightPosition - objectPosition;
		f32 distanceSquared = l.LengthSquared();
		// see Real Shading in Unreal Engine 4.
		f32 falloff = Square(Clamp(1 - Square(distanceSquared / Square(radius_)), 0.f, 1.f)) / (distanceSquared * inverseScaleSquare_ + 1);
		assert(0.f <= falloff && falloff <= 1.f);
		return intensity_ * falloff;
	}


}

