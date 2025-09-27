// 04.03
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "CoreMinimal.h"

struct FTask
{
	FTask(int InPriority, std::function<void()> InWork, std::atomic<bool>* InIsWorkComplete)
		: Priority(InPriority), Work(InWork), IsWorkComplete(InIsWorkComplete) { }

	int                   Priority;
	std::function<void()> Work;
	std::atomic<bool>*    IsWorkComplete;
	std::function<void()> WorkCompleteCallback;

	bool operator<(const FTask& Other) const
	{
		return Priority < Other.Priority;
	}
};

// 언리얼엔진에서는 인터페이스로서 사용되지만
// LeeEngine에서는 자체적으로 쓰레드풀을 제공하는 역할로 진행
// std::condition_variable을 통해 작업자 쓰레드를 대기상태로 유지할 수 있도록 조정
class FQueuedThreadPool
{
public:
	FQueuedThreadPool(UINT InNumQueuedThreads);
	~FQueuedThreadPool();

	void AddTask(const FTask& NewTask);

	UINT GetThreadCount() const
	{
		return static_cast<UINT>(Threads.size());
	}

private:
	concurrency::concurrent_priority_queue<FTask> TaskQueue;
	std::vector<std::thread>                      Threads;
	std::atomic<bool>                             bStop = false;
	std::mutex                                    QueueMutex;
	std::condition_variable                       Condition;
};

extern std::unique_ptr<FQueuedThreadPool> GThreadPool;

void ParallelFor(UINT TaskCount, UINT Priority, const std::function<void(int)>& Body);
