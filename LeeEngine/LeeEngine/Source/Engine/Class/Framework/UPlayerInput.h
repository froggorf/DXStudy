// 07.10
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/UObject/UObject.h"

enum class EKeys : uint16_t
{
    // None
    None = 0,

    // 알파벳
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    // 숫자(상단)
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,

    // 숫자패드
    Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
    Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
    NumpadAdd, NumpadSubtract, NumpadMultiply, NumpadDivide, NumpadDecimal,

    // 방향키
    Up, Down, Left, Right,

    // 마우스 버튼
    MouseLeft, MouseRight, MouseMiddle,
    MouseXButton1, MouseXButton2,
    MouseWheelUp, MouseWheelDown,

    // 펑션키
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

    // 특수키
    Space, Tab, Enter, Escape, Backspace, Insert, Delete,
    Home, End, PageUp, PageDown, CapsLock, ScrollLock, PauseBreak,
    PrintScreen, Menu, Apps,

    // Modifier
    LShift, RShift, LCtrl, RCtrl, LAlt, RAlt,

    // 최대값 (터치 등 개수 조정시 사용)
    NUM_KEYS
};



enum ETriggerEvent : int
{
    Trigger              =0,
    Released             =1,
    Started              =2,
    MAX                  =5,
};

struct FInputEvent
{
    EKeys Key;
    bool bKeyDown;

    XMFLOAT2 CurPosition;
    XMFLOAT2 Delta;
    float WheelData;
};

struct KeyEventHash
{
    std::size_t operator()(const std::pair<EKeys, ETriggerEvent>& k) const
    {
        return std::hash<int>()(static_cast<int>(k.first)) ^ (std::hash<int>()(static_cast<int>(k.second)) << 1);
    }
};

class UPlayerInput : public UObject
{
	MY_GENERATE_BODY(UPlayerInput)
public:
    // 키가 눌려있는지를 체크하는 변수
	std::array<bool, static_cast<UINT>(EKeys::NUM_KEYS)> Touches;

    // 바인딩 액션을 관리하는 변수
    std::unordered_map<std::pair<EKeys, ETriggerEvent>, std::function<void()>,KeyEventHash> BindEvents;

    // Input을 처리하는 함수
    void HandleInput(const FInputEvent& InputEvent);

    template<typename T>
    void BindAction(EKeys Key, ETriggerEvent TriggerEvent, T* Object, void (T::*Func)())
    {
        BindEvents[{Key, TriggerEvent}] = [Object, Func]()
            {
                (Object->*Func)();
            };
    }

    template<typename T>
    void BindAxis(EKeys Key, ETriggerEvent TriggerEvent, float Val, T* Object, void (T::*Func)(float))
    {
        BindEvents[{Key, TriggerEvent}] = [Object, Func, Val]()
            {
                (Object->*Func)(Val);
            };
    }

    template<typename T>
    void BindAxis2D(EKeys Key, ETriggerEvent TriggerEvent, float ValX, float ValY, T* Object, void (T::*Func)(float,float))
    {
        BindEvents[{Key, TriggerEvent}] = [Object, Func, ValX, ValY]()
            {
                (Object->*Func)(ValX,ValY);
            };
    }
};
