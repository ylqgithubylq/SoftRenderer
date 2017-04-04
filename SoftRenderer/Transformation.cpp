#include "Header.hpp"
#include "Transformation.hpp"

namespace X
{
	Transformation::Transformation()
		: scaling_(1), position_(f32V3(0, 0, 0)), orientation_(f32M44(1.f)),
		front_(0, 0, 1), up_(0, 1, 0)
	{
	}


	Transformation::~Transformation()
	{
	}

	f32M44 Transformation::GetWorldMatrix() const
	{
		return ScalingMatrix(scaling_) * (orientation_) * TranslationMatrix(position_);
	}

}

