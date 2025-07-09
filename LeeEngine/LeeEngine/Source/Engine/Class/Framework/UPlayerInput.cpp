#include "CoreMinimal.h"
#include "UPlayerInput.h"

void UPlayerInput::HandleInput(const FInputEvent& InputEvent)
{
	// 키 누름
	UINT KeyIndex = static_cast<UINT>(InputEvent.Key);
	if (InputEvent.bKeyDown)
	{
		if (!Touches[KeyIndex])
		{
			if (KeyIndex == static_cast<UINT>(EKeys::W))
			{
				int a= 0;
			}
			Touches[KeyIndex] = true;
			// Start에 대한 처리하기

			auto StartEvent = BindEvents.find({InputEvent.Key, ETriggerEvent::Started});
			if (StartEvent != BindEvents.end())
			{
				StartEvent->second();
			}
		}

		// Trigger에 대한 처리하기
		auto TriggerEvent = BindEvents.find({InputEvent.Key, ETriggerEvent::Trigger});
		if (TriggerEvent != BindEvents.end())
		{
			TriggerEvent->second();
		}
	}
	else
	{
		if (Touches[KeyIndex])
		{
			Touches[KeyIndex] = false;

			auto ReleaseEvent = BindEvents.find({InputEvent.Key, ETriggerEvent::Released});
			if (ReleaseEvent != BindEvents.end())
			{
				ReleaseEvent->second();
			}
		}
	}
	
}
