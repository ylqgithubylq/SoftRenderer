#pragma once
#include "BasicType.hpp"
#include "Utility.hpp"

#include <thread>
#include <condition_variable>
#include <future>
#include <atomic>
#include <queue>

namespace X
{
	class ThreadedTaskPool
		: Noncopyable
	{
	public:
		ThreadedTaskPool();
		~ThreadedTaskPool();

		void Enqueue(std::function<void()>&& task);

		void LaunchAndWait();

	private:
		void Run();
		void WaitAll();

		bool TryGetTask(std::function<void()> const** outTask);


		struct ThreadWorker
			: Noncopyable
		{
			ThreadWorker(ThreadedTaskPool& pool);

			void Start();
			void Stop();

			void WorkingLoop();

			void Join();

			bool Working();
			void StartWork();
			void FinishWork();

			ThreadedTaskPool& pool_;
			std::thread thread_;
			bool running_;
			bool working_;
			std::mutex mutex_;
		};

		void WorkerThreadWait(ThreadWorker& worker);


	private:
		u32 threadCount_;
		std::atomic<s32> undoneTaskCount_;

		std::atomic<bool> finish_;
		std::atomic<u32> notFinishedWorkerCount_;

		std::mutex finishMutex_;
		std::condition_variable finishCV_;
		std::unique_lock<std::mutex> finishLock_;

		std::vector<std::function<void()>> tasks_;
		std::vector<std::unique_ptr<ThreadWorker>> workers_;

		std::mutex threadWaitingMutex_;
		std::condition_variable threadWaitingCV_;
		std::unique_lock<std::mutex> threadWaitingLock_;

	};
}


