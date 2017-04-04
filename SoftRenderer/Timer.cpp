#include "Header.hpp"
#include "Timer.hpp"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif
#include <windows.h>

#ifdef max // windows.h defined this...
#undef max
#endif

namespace X
{
	namespace
	{
		u64 CPS()
		{
			static u64 cps = 0;
			if (0 == cps)
			{
				LARGE_INTEGER frequency;
				QueryPerformanceFrequency(&frequency);
				cps = static_cast<u64>(frequency.QuadPart);
			}
			return cps;
		}
	}


	Timer::Timer()
	{
		this->Restart();
	}
	Timer::~Timer()
	{
	}

	void Timer::Restart()
	{
		startTime_ = CurrentTime();
	}

	f64 Timer::Elapsed() const
	{
		return CurrentTime() - startTime_;
	}

	f64 Timer::MaxElapseTime() const
	{
		return static_cast<f64>(std::numeric_limits<u64>::max()) / CPS() - startTime_;
	}

	f64 Timer::MinElapseTimeSpan() const
	{
		return 1.0 / CPS();
	}

	f64 Timer::CurrentTime() const
	{
		LARGE_INTEGER count;
		QueryPerformanceCounter(&count);
		return static_cast<double>(count.QuadPart) / CPS();
	}
}

