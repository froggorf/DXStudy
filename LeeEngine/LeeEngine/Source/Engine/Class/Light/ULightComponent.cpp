#include "CoreMinimal.h"
#include "ULightComponent.h"

#include "Engine/Physics/UCapsuleComponent.h"
#include "Engine/World/UWorld.h"

void ULightComponent::Register()
{
	USceneComponent::Register();

	GEngine->GetWorld()->AddCurrentFrameLightInfo(Info);

	GEngine->GetWorld()->DrawDebugBox(GetWorldLocation(), {GetRadius(),GetRadius(),GetRadius()}, GetLightColor(), {0,0,0,1}, 100);
}

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
