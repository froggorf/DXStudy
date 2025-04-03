#include "CoreMinimal.h"
#include "QueuedThreadPool.h"

std::unique_ptr<FQueuedThreadPool> GThreadPool = nullptr;

FQueuedThreadPool::FQueuedThreadPool(UINT InNumQueuedThreads)
{
	for(int i = 0; i < InNumQueuedThreads; ++i)
	{
		Threads.emplace_back([this]()
			{
				while(true)
				{
					FTask CurrentTask{0, nullptr,nullptr};

					// try_pop
					{
						std::unique_lock<std::mutex> Lock(QueueMutex);
						// 종료되거나 작업이 생기는 시점까지 대기
						Condition.wait(Lock, [this](){return bStop || !TaskQueue.empty();});

						if(bStop && TaskQueue.empty()) return;

						if(!TaskQueue.try_pop(CurrentTask)) continue;
					}
					CurrentTask.Work();
					*(CurrentTask.IsWorkComplete) = true;
				}
			});
	}
}

FQueuedThreadPool::~FQueuedThreadPool()
{
	bStop =true;
	Condition.notify_all();
	for(auto& Thread : Threads)
	{
		if(Thread.joinable())
		{
			Thread.join();
		}
	}
}

void FQueuedThreadPool::AddTask(const FTask& NewTask)
{
	TaskQueue.push(NewTask);

	Condition.notify_one();
}
