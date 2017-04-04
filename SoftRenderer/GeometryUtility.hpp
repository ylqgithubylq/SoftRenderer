#pragma once
#include "Common.hpp"

namespace X
{
	std::shared_ptr<GeometryLayout> MakeCube(f32 halfSize);

	std::shared_ptr<GeometryLayout> MakeCone(f32 radius, f32 height, u32 radiusSplittingCount);
}

