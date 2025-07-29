#include "CoreMinimal.h"
#include "ULightComponent.h"

#include "Engine/World/UWorld.h"

void ULightComponent::SetLightType(ELightType Type)
{
	Info.LightType = static_cast<int>(Type);

}

void ULightComponent::SetRadius(float Radius)
{
	Info.Radius = Radius;
}

void ULightComponent::TickComponent(float DeltaSeconds)
{
	USceneComponent::TickComponent(DeltaSeconds);

	if (bActive)
	{
		GEngine->GetWorld()->AddCurrentFrameLightInfo(Info);	
	}
}
