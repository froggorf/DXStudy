#include "CoreMinimal.h"
#include "QueuedThreadPool.h"

#include "Engine/RenderCore/RenderingThread.h"

std::unique_ptr<FQueuedThreadPool> GThreadPool = nullptr;

FQueuedThreadPool::FQueuedThreadPool(UINT InNumQueuedThreads)
{
	for (UINT i = 0; i < InNumQueuedThreads; ++i)
	{
		std::thread NewWorkerThread = std::thread([this]()
			{
				while (true)
				{
					FTask CurrentTask{0, nullptr, nullptr};

					if (bIsGameKill)
					{
						return;
					}

					// try_pop
					{
						if (TaskQueue.try_pop(CurrentTask))
						{
							CurrentTask.Work();

							if(CurrentTask.IsWorkComplete)
							{
								CurrentTask.IsWorkComplete->store(true);	
							}

							if (bIsGameKill)
							{
								return;
							}

							if(CurrentTask.WorkCompleteCallback)
							{
								CurrentTask.WorkCompleteCallback();	
							}

							continue;
						}
						// 종료되거나 작업이 생기는 시점까지 대기
						std::unique_lock<std::mutex> Lock(QueueMutex);
						Condition.wait(Lock,
							[this]()
							{
								return bStop || !TaskQueue.empty();
							});

						if (bStop && TaskQueue.empty())
							return;
						if (bIsGameKill)
						{
							return;
						}
					}
				}
			});

		SetThreadPriority(NewWorkerThread.native_handle(), THREAD_PRIORITY_LOWEST); 
		Threads.emplace_back(std::move(NewWorkerThread));
	}
}

FQueuedThreadPool::~FQueuedThreadPool()
{
	
}

void FQueuedThreadPool::AddTask(const FTask& NewTask)
{
	TaskQueue.push(NewTask);

	Condition.notify_one();
}

void FQueuedThreadPool::ClearAllThread()
{
	bStop = true;
	Condition.notify_all();
	for (auto& Thread : Threads)
	{
		Thread.detach();
		//if (Thread.joinable())
		//{
		//	Thread.join();
		//}
	}
}

// =================================================

void ParallelFor(UINT TaskCount, UINT Priority, const std::function<void(int)>& Body)
{
	const UINT ThreadCount = GThreadPool->GetThreadCount();
	// 싱글쓰레드 가동
	if (ThreadCount == 0)
	{
		for (UINT i = 0; i < TaskCount; ++i)
		{
			Body(i);
		}
		return;
	}

	const UINT TasksPerThread = (TaskCount + ThreadCount - 1) / ThreadCount;

	std::vector<std::shared_ptr<std::atomic<bool>>> IsWorkComplete(ThreadCount);
	for (UINT i = 0; i < ThreadCount; ++i)
	{
		IsWorkComplete[i] = std::make_shared<std::atomic<bool>>(false);
	}

	for (UINT i = 0; i < ThreadCount; ++i)
	{
		UINT StartIndex = i * TasksPerThread;
		UINT EndIndex   = std::min(StartIndex + TasksPerThread, TaskCount);

		std::atomic<bool>& WorkCompleteFlag = *IsWorkComplete[i];
		GThreadPool->AddTask(FTask{
			static_cast<int>(Priority), [StartIndex, EndIndex, &Body]()
			{
				for (UINT i = StartIndex; i < EndIndex; ++i)
				{
					Body(i);
				}
			},
			&WorkCompleteFlag
		});
	}
	while (true)
	{
		bool bAllTasksComplete = true;
		for (UINT i = 0; i < ThreadCount; ++i)
		{
			if (!IsWorkComplete[i]->load())
			{
				bAllTasksComplete = false;
				break;
			}
		}
		if (bAllTasksComplete)
			break;
		std::this_thread::yield();
	}
}
