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
