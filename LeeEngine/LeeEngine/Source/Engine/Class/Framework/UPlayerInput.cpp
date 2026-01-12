#include "CoreMinimal.h"
#include "UPlayerInput.h"

wchar_t KeyToChar(EKeys Key, bool bShiftPressed, bool bCapsLockOn)
{
    // 알파벳 (A-Z)
    if (Key >= EKeys::A && Key <= EKeys::Z)
    {
        int Offset = static_cast<int>(Key) - static_cast<int>(EKeys::A);

        // Shift 또는 CapsLock이 켜져있으면 대문자
        bool bUpperCase = (bShiftPressed != bCapsLockOn);  // XOR 연산

        if (bUpperCase)
        {
            return L'A' + Offset;
        }
        else
        {
            return L'a' + Offset;
        }
    }

    // 숫자 키 (상단 0-9)
    if (Key >= EKeys::Num0 && Key <= EKeys::Num9)
    {
        int Offset = static_cast<int>(Key) - static_cast<int>(EKeys::Num0);

        // Shift 없으면 숫자
        if (!bShiftPressed)
        {
            return L'0' + Offset;
        }
        // Shift 있으면 특수문자
        else
        {
            switch (Key)
            {
            case EKeys::Num1: return L'!';
            case EKeys::Num2: return L'@';
            case EKeys::Num3: return L'#';
            case EKeys::Num4: return L'$';
            case EKeys::Num5: return L'%';
            case EKeys::Num6: return L'^';
            case EKeys::Num7: return L'&';
            case EKeys::Num8: return L'*';
            case EKeys::Num9: return L'(';
            case EKeys::Num0: return L')';
            default: return L'\0';
            }
        }
    }

    // 숫자패드 (0-9)
    if (Key >= EKeys::Numpad0 && Key <= EKeys::Numpad9)
    {
        int Offset = static_cast<int>(Key) - static_cast<int>(EKeys::Numpad0);
        return L'0' + Offset;
    }

    // 숫자패드 연산자
    switch (Key)
    {
    case EKeys::NumpadAdd:      return L'+';
    case EKeys::NumpadSubtract: return L'-';
    case EKeys::NumpadMultiply: return L'*';
    case EKeys::NumpadDivide:   return L'/';
    case EKeys::NumpadDecimal:  return L'.';
    default: break;
    }

    // 공백 및 기본 키
    if (Key == EKeys::Space)     return L' ';
    if (Key == EKeys::Tab)       return L'\t';
    if (Key == EKeys::Enter)     return L'\n';


    return L'\0';  // 변환 불가능한 키
}

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
void UPlayerInput::ClearInputState()                                                                          
{                                                                                                             
    std::fill(Touches.begin(), Touches.end(), false);                                           
}   