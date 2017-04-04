#pragma once

#include "Common.hpp"

namespace X
{
	struct Setting
	{
		u32 left;
		u32 top;
		u32 width;
		u32 height;
		std::wstring title;
		std::string rootPath;
		u32 threadSupport; // 0 indicates maximum thread support.
	};
}

