#include "CoreMinimal.h"
#include "ULightComponent.h"

#include "Engine/Physics/UCapsuleComponent.h"
#include "Engine/World/UWorld.h"


void ULightComponent::Register()
{
	USceneComponent::Register();

	Rename("LightComp" + std::to_string(ComponentID));
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


void UDecalComponent::TickComponent(float DeltaSeconds)
{
	USceneComponent::TickComponent(DeltaSeconds);

	if (bIsActive)
	{
		AddDecalInfo();

	}
}

void UDecalComponent::Tick_Editor(float DeltaSeconds)
{
	USceneComponent::Tick_Editor(DeltaSeconds);

	if (bIsActive)
	{
		AddDecalInfo();
		const FTransform& CurrentTransform = GetComponentTransform();
		GEngine->GetWorld()->DrawDebugBox(GetWorldLocation(), {CurrentTransform.GetScale3D()}, XMFLOAT3{0.0f,1.0f,0.0f}, CurrentTransform.GetRotationQuat(), DeltaSeconds);
	}
}

void UDecalComponent::AddDecalInfo()
{
	FDecalInfo DecalInfo;
	DecalInfo.Transform = GetComponentTransform();
	DecalInfo.DecalTexture = DecalTexture.get();
	DecalInfo.bIsLight = static_cast<int>(bIsLight);
	GEngine->GetWorld()->AddCurrentFrameDecalInfo(DecalInfo);
}
