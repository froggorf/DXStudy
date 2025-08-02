#include "CoreMinimal.h"
#include "ULightComponent.h"

#include "Engine/Physics/UCapsuleComponent.h"
#include "Engine/World/UWorld.h"

void ULightComponent::Register()
{
	USceneComponent::Register();

	AddLightInfo();	

	
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
		AddLightInfo();

	}
}

void ULightComponent::Tick_Editor(float DeltaSeconds)
{
	USceneComponent::Tick_Editor(DeltaSeconds);


	if (bActive)
	{
		AddLightInfo();
		GEngine->GetWorld()->DrawDebugBox(GetWorldLocation(), {GetRadius(),GetRadius(),GetRadius()}, GetLightColor(), {0,0,0,1}, DeltaSeconds);
	}
}

void ULightComponent::AddLightInfo()
{
	Info.WorldPos = GetWorldLocation();
	Info.WorldDir = GetForwardVector();
	GEngine->GetWorld()->AddCurrentFrameLightInfo(Info);
}
