#include "CoreMinimal.h"
#include "UAnimSequenceBase.h"

void UAnimSequenceBase::SortNotifies()
{
	std::sort(Notifies.begin(),Notifies.end());
}

void UAnimSequenceBase::GetAnimNotifies(const float& StartTime, const float& DeltaTime,
	std::vector<FAnimNotifyEvent>& OutActiveNotifies)
{
	// early out
	if(Notifies.size() == 0)
	{
		return;
	}

	float PreviousPosition = StartTime;
	float CurrentPosition = StartTime + DeltaTime;
	for(const auto& NotifyEvent : Notifies)
	{
		const float NotifyStartTime = NotifyEvent.GetTriggerTime();
		const float NotifyEndTime = NotifyEvent.GetEndTriggerTime();
		if(NotifyStartTime <= CurrentPosition && NotifyEndTime>PreviousPosition)
		{
			OutActiveNotifies.emplace_back(NotifyEvent);
		}
	}
}
