#include "CoreMinimal.h"
#include "UAnimSequenceBase.h"

#include "Engine/RenderCore/EditorScene.h"

void UAnimSequenceBase::SortNotifies()
{
	std::sort(Notifies.begin(), Notifies.end());
}

void UAnimSequenceBase::GetAnimNotifies(const float& CurrentTime, std::vector<FAnimNotifyEvent>& OutActiveNotifies)
{
	// early out
	if (Notifies.size() == 0)
	{
		return;
	}

	// 급격한 애니메이션 변화로 인한 AnimNotify 중복 호출 방어
	if (LastUpdateTimeSeconds + 0.1f > GEngine->GetTimeSeconds())
	{
		float PreviousPosition = LastUpdateTime;
		float CurrentPosition  = CurrentTime;
		for (const auto& NotifyEvent : Notifies)
		{
			const float NotifyStartTime = NotifyEvent.GetTriggerTime();
			const float NotifyEndTime   = NotifyEvent.GetEndTriggerTime();
			if (NotifyStartTime <= CurrentPosition && NotifyEndTime > PreviousPosition)
			{
				OutActiveNotifies.emplace_back(NotifyEvent);
			}
		}
		LastUpdateTime = CurrentPosition;
	}
	LastUpdateTimeSeconds = GEngine->GetTimeSeconds();
}

void UAnimSequenceBase::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UAnimationAsset::LoadDataFromFileData(AssetData);
	if (AssetData.contains("Notifies"))
	{
		const auto& NotifiesData = AssetData["Notifies"];
		for (const auto& NotifyData : NotifiesData)
		{
			FAnimNotifyEvent NotifyEvent;
			if (NotifyData.contains("NotifyClass"))
			{
				std::string NotifyClassName = NotifyData["NotifyClass"];
				NotifyEvent.Notify          = std::dynamic_pointer_cast<UAnimNotify>(GetDefaultObject(NotifyClassName)->CreateInstance());
				NotifyEvent.Notify->LoadDataFromFileData(NotifyData);
			}
			NotifyEvent.TriggerTimeOffset = NotifyData["TriggerTime"];
			// Json 에러로 인하여 분리
			//NotifyEvent.EndTriggerTimeOffset = NotifyData.contains("EndTriggerTime") ? NotifyData["EndTriggerTime"] : NotifyEvent.TriggerTimeOffset;
			if (NotifyData.contains("EndTriggerTime"))
			{
				NotifyEvent.EndTriggerTimeOffset = NotifyData["EndTriggerTime"];
			}
			else
			{
				NotifyEvent.EndTriggerTimeOffset = NotifyEvent.TriggerTimeOffset;
			}

			Notifies.emplace_back(NotifyEvent);
		}
	}
}
