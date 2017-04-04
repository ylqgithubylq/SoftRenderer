#include "Header.hpp"
#include "PerformanceCounter.hpp"
#include "Context.hpp"
#include "Timer.hpp"

#include <thread>

#include <ppl.h>

namespace X
{
	struct PerformanceCounter::Impl
	{
		Impl(PerformanceCounter& counter, Context& context)
			: counter_(counter), context_(context)
		{
		}

		PerformanceCounter& counter_;
		Context& context_;

		struct CounterStruct
		{
			concurrency::combinable<f64> accumulator;
			concurrency::combinable<f64> startTime;
		};
		std::array<CounterStruct, static_cast<u32>(Term::TermCount)> counters_;
	};


	PerformanceCounter::PerformanceCounter(Context& context)
	{
		impl_ = std::make_unique<Impl>(*this, context);

	}


	PerformanceCounter::~PerformanceCounter()
	{
	}

	void PerformanceCounter::Begin(Term term)
	{
		impl_->counters_[static_cast<u32>(term)].startTime.local() = impl_->context_.GetElapsedTime();
	}
	void PerformanceCounter::End(Term term)
	{
		impl_->counters_[static_cast<u32>(term)].accumulator.local() += impl_->context_.GetElapsedTime() - impl_->counters_[static_cast<u32>(term)].startTime.local();		
	}
	f32 PerformanceCounter::Get(Term term)
	{
		f64 t = impl_->counters_[static_cast<u32>(term)].accumulator.combine([] (f64 left, f64 right)
		{
			return left + right;
		});

		return f32(t);
	}

	f64 PerformanceCounter::GetStartTime(Term term)
	{
		f64 total = 0;
		u32 count = 0;
		impl_->counters_[static_cast<u32>(term)].startTime.combine_each([&total, &count] (f64 v)
		{
			total += v;
			count += 1;
		});
		return total / count;
	}

	void PerformanceCounter::Clear(Term term)
	{
		impl_->counters_[static_cast<u32>(term)].accumulator.clear();
	}

	void PerformanceCounter::ClearAll()
	{
		for (u32 i = 0; i < static_cast<u32>(Term::TermCount); ++i)
		{
			Clear(static_cast<Term>(i));
		}
	}



}

