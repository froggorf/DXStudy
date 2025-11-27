#include "CoreMinimal.h"
#include "UPlayerInput.h"

void UPlayerInput::BeginPlay()
{
	UObject::BeginPlay();

	
}

void UPlayerInput::Tick()
{
	for (auto EventIter = BindEvents.begin(); EventIter != BindEvents.end(); ++EventIter)
	{
		// Trigger면
		if (EventIter->first.second == ETriggerEvent::Trigger)
		{
			// 키가 누르는 중이라면
			if (Touches[static_cast<UINT>(EventIter->first.first)])
			{
				// Trigger Bind Event 실행
				EventIter->second();
			}
		}
	}
}

void UPlayerInput::HandleInput(const FInputEvent& InputEvent)
{
	// 마우스 2DAxis에 대한 처리는 별도로 진행
	if (InputEvent.Key == EKeys::MouseXY2DAxis)
	{
		auto MouseXY2dAxisEvent = BindEvents.find({EKeys::MouseXY2DAxis, ETriggerEvent::Trigger});
		if (MouseXY2dAxisEvent != BindEvents.end())
		{
			MouseXY2dAxisEvent->second();
		}
		return;
	}

	// 키 누름
	UINT KeyIndex = static_cast<UINT>(InputEvent.Key);
	if (InputEvent.bKeyDown)
	{
		if (!Touches[KeyIndex])
		{
			if (InputEvent.Key == EKeys::MouseWheelDown || InputEvent.Key == EKeys::MouseWheelUp)
			{
				// 휠 버튼에 대해서는 처리를 하지 말아야함
			}
			else
			{
				Touches[KeyIndex] = true;	
			}
			

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
