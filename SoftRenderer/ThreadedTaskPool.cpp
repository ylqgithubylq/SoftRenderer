#include "Header.hpp"
#include "ThreadedTaskPool.hpp"




namespace X
{
	ThreadedTaskPool::ThreadedTaskPool()
		: threadWaitingLock_(threadWaitingMutex_, std::defer_lock), finishLock_(finishMutex_, std::defer_lock)
	{
		finish_.store(true);
		threadCount_ = std::thread::hardware_concurrency();
		if (threadCount_ == 0)
		{
			threadCount_ = 1;
		}
		notFinishedWorkerCount_.store(threadCount_);

		for (u32 i = 0; i < threadCount_; ++i)
		{
			workers_.push_back(std::unique_ptr<ThreadWorker>(new ThreadWorker(*this)));
		}
		for (auto& worker : workers_)
		{
			worker->Start();
		}
		WaitAll();
	}


	ThreadedTaskPool::~ThreadedTaskPool()
	{
		assert(finish_ == true);
		for (auto& worker : workers_)
		{
			worker->Stop();
			worker->StartWork();
		}
		Run();
		for (auto& worker : workers_)
		{
			worker->Join();
		}
	}

	void ThreadedTaskPool::Enqueue(std::function<void()>&& task)
	{
		assert(finish_ == true);
		tasks_.push_back(std::move(task));
	}

	void ThreadedTaskPool::LaunchAndWait()
	{
		Run();
		WaitAll();
	}

	void ThreadedTaskPool::Run()
	{
		assert(finish_ == true);
		assert(notFinishedWorkerCount_ == 0);
		undoneTaskCount_.store(tasks_.size());
		finish_.store(false);
		notFinishedWorkerCount_.store(threadCount_);
		for (auto& worker : workers_)
		{
			worker->StartWork();
		}
		threadWaitingCV_.notify_all();
	}


	void ThreadedTaskPool::WaitAll()
	{
		std::lock_guard<std::unique_lock<std::mutex>> guard(finishLock_);
		finishCV_.wait(finishLock_, [this] ()
		{
			return finish_.load() == true; // until finish == true (all workers are done)
		});
		tasks_.clear();
	}

	bool ThreadedTaskPool::TryGetTask(std::function<void()> const** outTask)
	{
		s32 index = s32(tasks_.size()) - undoneTaskCount_.fetch_sub(1, std::memory_order_acq_rel);
		if (index < s32(tasks_.size())) // to avoid u32 underflow
		{
			*outTask = &tasks_[index];
			assert(*outTask != nullptr);
			return true;
		}
		return false;
	}

	void ThreadedTaskPool::WorkerThreadWait(ThreadWorker& worker)
	{
		std::lock_guard<std::unique_lock<std::mutex>> lockGuard(threadWaitingLock_);
		u32 notFinishedWorkerCount = --notFinishedWorkerCount_;
		worker.FinishWork();
		if (notFinishedWorkerCount == 0)
		{
			finish_.store(true);
			finishCV_.notify_all();
		}

		threadWaitingCV_.wait(threadWaitingLock_, [&worker] ()
		{
			return worker.Working(); // until worker is working
		});
	}

	ThreadedTaskPool::ThreadWorker::ThreadWorker(ThreadedTaskPool& pool)
		: pool_(pool), running_(true), working_(false)
	{
	}



	void ThreadedTaskPool::ThreadWorker::WorkingLoop()
	{
		while (running_)
		{
			std::function<void()> const* task = nullptr;
			if (pool_.TryGetTask(&task))
			{
				(*task)();
			}
			else
			{
				pool_.WorkerThreadWait(*this);
			}
		}
	}

	void ThreadedTaskPool::ThreadWorker::Start()
	{
		thread_.swap(std::thread([this] ()
		{
			WorkingLoop();
		}));
	}

	void ThreadedTaskPool::ThreadWorker::Stop()
	{
		running_ = false;
	}

	void ThreadedTaskPool::ThreadWorker::Join()
	{
		thread_.join();
	}

	bool ThreadedTaskPool::ThreadWorker::Working()
	{
		return working_;
	}

	void ThreadedTaskPool::ThreadWorker::StartWork()
	{
		working_ = true;
	}

	void ThreadedTaskPool::ThreadWorker::FinishWork()
	{
		working_ = false;
	}

}

