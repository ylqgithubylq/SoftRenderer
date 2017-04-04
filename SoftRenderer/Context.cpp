#include "Header.hpp"
#include "Context.hpp"
#include "MainWindow.hpp"
#include "Renderer.hpp"
#include "InputManager.hpp"
#include "ResourceLoader.hpp"
#include "MainWindow.hpp"
#include "Scene.hpp"
#include "PerformanceCounter.hpp"

#include <string>
#include <ppl.h>

namespace X
{
	namespace
	{
		static const u32 TileSize = 64;
	}

	namespace
	{
		void SetPPLConcurrency(u32 thread)
		{
			if (thread == 0)
			{
				concurrency::CurrentScheduler::Detach();
				concurrency::CurrentScheduler::Create(concurrency::SchedulerPolicy(1, concurrency::MaxConcurrency, concurrency::MaxExecutionResources));
				//concurrency::Scheduler::SetDefaultSchedulerPolicy(concurrency::SchedulerPolicy(1, concurrency::MaxConcurrency, concurrency::MaxExecutionResources));
			}
			else if (thread != 1)
			{
				concurrency::CurrentScheduler::Detach();
				concurrency::CurrentScheduler::Create(concurrency::SchedulerPolicy(1, concurrency::MaxConcurrency, thread - 1));
				//concurrency::Scheduler::SetDefaultSchedulerPolicy(concurrency::SchedulerPolicy(1, concurrency::MaxConcurrency, thread - 1));
			}
		}
	}

	Context::Context(Setting const& setting)
		:setting_(setting), lastFrameTime_(0)
	{
		// round to multiple times of tile size
		setting_.width = setting_.width / TileSize * TileSize;
		setting_.height = setting_.height / TileSize * TileSize;

		window_ = std::make_unique<MainWindow>(setting_);
		
		inputManager_ = std::make_unique<InputManager>(*this);
		window_->SetInputManager(inputManager_.get());

		resourceLoader_ = std::make_unique<ResourceLoader>(setting_.rootPath);

		renderer_ = std::make_unique<Renderer>(*this);

		scene_ = std::make_unique<Scene>();

		performanceCounter_ = std::make_unique<PerformanceCounter>(*this);

		concurrency::CurrentScheduler::Create(concurrency::SchedulerPolicy(1, concurrency::MaxConcurrency, concurrency::MaxExecutionResources));
		SetPPLConcurrency(GetThreadSupport());
	}


	Context::~Context()
	{
		// give them an order
		renderer_ = nullptr;
		inputManager_ = nullptr;
		window_ = nullptr;
	}

	void Context::Start()
	{
		bool running = true;

		timer_.Restart();
		lastFrameTime_ = timer_.Elapsed();

		fps_ = 0;
		std::queue<f64> frameTimes;
		frameTimes.push(lastFrameTime_);

		Size<u32, 2> size = window_->GetClientRegionSize();
		do
		{
			f64 current = timer_.Elapsed();
			f32 delta = f32(current - lastFrameTime_);

			while (frameTimes.size() > 1 && current - frameTimes.front() > 1)
			{
				frameTimes.pop();
			}
			fps_ = f32(frameTimes.size() / (current - frameTimes.front()));
			frameTimes.push(current);

			if (logic_ != nullptr)
			{
				running = logic_(current, delta) && running; // logic call first to ensure a run
			}

			performanceCounter_->ClearAll();
			performanceCounter_->Begin(PerformanceCounter::Term::All);

			running = window_->HandleMessage();

			inputManager_->ExecuteAllQueuedActions(current);


			performanceCounter_->Begin(PerformanceCounter::Term::Render);
			renderer_->RenderAFrame(current, delta);
			performanceCounter_->End(PerformanceCounter::Term::Render);
			
			Size<u32, 2> colorBufferSize = renderer_->GetColorBuffer().GetSize(0);			
			assert(colorBufferSize.X() == size.X() && colorBufferSize.Y() == size.Y());
			f32V3 const* colors = renderer_->GetColorBuffer().GetValues(0);
			
			window_->DrawColorRectangle(colors, size.X(), size.Y());

			lastFrameTime_ = current;

			performanceCounter_->End(PerformanceCounter::Term::All);
		}
		while (running);
	}

	void Context::SetThreadSupport(u32 thread)
	{
		setting_.threadSupport = thread;
		SetPPLConcurrency(thread);
	}

}

