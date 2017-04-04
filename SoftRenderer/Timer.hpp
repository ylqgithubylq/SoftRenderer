#pragma once
#include "Common.hpp"

namespace X
{
	class Timer
		: Noncopyable
	{
	public:
		Timer();

		~Timer();

		void Restart();
		/*
		*	@return: time span since last call to Restart, or since creation.
		*/
		f64 Elapsed() const;

		/*
		*	Maximum elapsed time can be supported.
		*/
		f64 MaxElapseTime() const;

		f64 MinElapseTimeSpan() const;

		f64 CurrentTime() const;

	private:
		f64 startTime_;
	};
}


