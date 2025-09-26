#include "CoreMinimal.h"
#include "FTimerManager.h"

std::atomic<UINT> TimerCount = 0;
FTimerHandle::FTimerHandle()
{
	TimerID = TimerCount++;
}
