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

	// 급격한 애니메이션 변화로 인한 AnimNotify 중복 호출 방어
	if(LastUpdateTimeSeconds + 0.1f > GEngine->GetTimeSeconds())
	{
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
	LastUpdateTimeSeconds = GEngine->GetTimeSeconds();
	
}
