#pragma once

#include "Common.hpp"
#include "Setting.hpp"

#include "Timer.hpp"

namespace X
{
	class Context
		: Noncopyable
	{
	public:
		Context(Setting const& setting);
		~Context();

		void SetLogic(std::function<bool(f64 current, f32 delta)> logic)
		{
			logic_ = std::move(logic);
		}

		/*
		*	Will return and stop running when logic return true.
		*/
		void Start();

		f64 GetElapsedTime() const
		{
			return timer_.Elapsed();
		}

		MainWindow& GetMainWindow() const
		{
			return *window_;
		}
		InputManager& GetInputManager() const
		{
			return *inputManager_;
		}
		ResourceLoader& GetResourceLoader() const
		{
			return *resourceLoader_;
		}
		Renderer& GetRenderer() const
		{
			return *renderer_;
		}
		Scene& GetScene() const
		{
			return *scene_;
		}
		PerformanceCounter& GetPerformanceCounter() const
		{
			return *performanceCounter_;
		}

		u32 GetThreadSupport() const
		{
			return setting_.threadSupport;
		}
		void SetThreadSupport(u32 thread);

		f32 GetFPS() const
		{
			return fps_;
		}

	private:
		Setting setting_;

		Timer timer_;
		f64 lastFrameTime_;

		f32 fps_;

		std::function<bool(f64 current, f32 delta)> logic_;

		std::unique_ptr<MainWindow> window_;
		std::unique_ptr<InputManager> inputManager_;
		std::unique_ptr<ResourceLoader> resourceLoader_;
		std::unique_ptr<Renderer> renderer_;
		std::unique_ptr<Scene> scene_;

		std::unique_ptr<PerformanceCounter> performanceCounter_;
	};
}


