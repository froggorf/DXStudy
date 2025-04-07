#include "CoreMinimal.h"
#include "UAnimSequenceBase.h"

#include "Engine/RenderCore/EditorScene.h"

void UAnimSequenceBase::SortNotifies()
{
	std::sort(Notifies.begin(),Notifies.end());
}

void UAnimSequenceBase::GetAnimNotifies(const float& CurrentTime,
	std::vector<FAnimNotifyEvent>& OutActiveNotifies)
{
	// early out
	if(Notifies.size() == 0)
	{
		return;
	}

	

	
	float PreviousPosition = LastUpdateTime;
	float CurrentPosition = CurrentTime;
	for(const auto& NotifyEvent : Notifies)
	{
		const float NotifyStartTime = NotifyEvent.GetTriggerTime();
		const float NotifyEndTime = NotifyEvent.GetEndTriggerTime();
		if(NotifyStartTime <= CurrentPosition && NotifyEndTime>PreviousPosition)
		{
			OutActiveNotifies.emplace_back(NotifyEvent);
		}
	}
	LastUpdateTime = CurrentPosition;
}
