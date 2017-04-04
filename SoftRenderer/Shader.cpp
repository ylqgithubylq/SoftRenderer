#include "Header.hpp"
#include "Shader.hpp"

#include <ppl.h>

namespace X
{

	namespace
	{
		Point<u32, 3> GetXYZFromFlatIndex(Size<u32, 3> const& size, u32 index)
		{
			u32 x = index % size.X();
			u32 yxz = index / size.X();
			u32 y = yxz % size.Y();
			u32 z = yxz / size.Y();

			return Point<u32, 3>(x, y, z);
		}
	}

	void ComputeLauncher::Launch(ComputeShader::WorkSize const& size, u32 threadSupport)
	{
		if (threadSupport == 1)
		{
			// per group
			for (u32 flatGroupIndex = 0; flatGroupIndex < size.groupCount.X() * size.groupCount.Y() * size.groupCount.Z(); ++flatGroupIndex)
			{
				Point<u32, 3> groupIndex = GetXYZFromFlatIndex(size.groupCount, flatGroupIndex);
				// per thread
				for (u32 flatThreadIndex = 0; flatThreadIndex < size.groupSize.X() * size.groupSize.Y() * size.groupSize.Z(); ++flatThreadIndex)
				{
					Point<u32, 3> threadIndex = GetXYZFromFlatIndex(size.groupSize, flatThreadIndex);
					(*shader_)(size, groupIndex, threadIndex, constantInput_, resources_);
				}
			}
		}
		else
		{
			// per group
			concurrency::parallel_for(0u, size.groupCount.X() * size.groupCount.Y() * size.groupCount.Z(), [this, &size] (u32 flatGroupIndex)
			{
				Point<u32, 3> groupIndex = GetXYZFromFlatIndex(size.groupCount, flatGroupIndex);
				// per thread
				concurrency::parallel_for(0u, size.groupSize.X() * size.groupSize.Y() * size.groupSize.Z(), [this, groupIndex, &size] (u32 flatThreadIndex)
				{
					Point<u32, 3> threadIndex = GetXYZFromFlatIndex(size.groupSize, flatThreadIndex);
					(*shader_)(size, groupIndex, threadIndex, constantInput_, resources_);
				});
			});
		}

	}

}

