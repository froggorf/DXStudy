#include "CoreMinimal.h"
#include "AnimNotify.h"

#include "Engine/RenderCore/EditorScene.h"

void UAnimNotify::Notify()
{
}

// =======================================

void UAnimNotify_PlaySound::Notify()
{
	MY_LOG("UAnimNotify_PlaySound", EDebugLogLevel::DLL_Warning, "PlaySound");
}

void UAnimNotify_PlaySound_WalkRight::Notify()
{
	MY_LOG("Walk", EDebugLogLevel::DLL_Warning, "       Right");
}

void UAnimNotify_PlaySound_RunLeft::Notify()
{

	MY_LOG("Run", EDebugLogLevel::DLL_Warning, "Left");
}

void UAnimNotify_PlaySound_RunRight::Notify()
{
	MY_LOG("Run", EDebugLogLevel::DLL_Warning, "       Right");
}

void UAnimNotify_PlaySound_WalkLeft::Notify()
{
	MY_LOG("Walk", EDebugLogLevel::DLL_Warning, "Left");
}
