#include "Header.hpp"
#include "Pipeline.hpp"
#include "Texture2D.hpp"

namespace X
{
	Pipeline::Pipeline(Renderer& renderer, Size<u32, 2> const& bufferSize)
		: renderer_(renderer), bufferSize_(bufferSize)
	{
	}


	Pipeline::~Pipeline()
	{
	}
}

