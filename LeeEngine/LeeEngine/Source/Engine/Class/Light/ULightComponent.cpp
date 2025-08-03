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
		GEngine->GetWorld()->DrawDebugBox(GetWorldLocation(), {CurrentTransform.GetScale3D().x/2,CurrentTransform.GetScale3D().y/2,CurrentTransform.GetScale3D().z/2}, XMFLOAT3{0.0f,1.0f,0.0f}, CurrentTransform.GetRotationQuat(), DeltaSeconds);
	}
}

void UDecalComponent::AddDecalInfo()
{
	FDecalInfo DecalInfo;
	DecalInfo.Transform = GetComponentTransform();
	DecalInfo.bIsLight = static_cast<int>(bIsLight);
	// 혹여 세팅이 안되어있을 경우에 대비하여 값을 설정해서 전달해줌
	DecalMaterial->SetRasterizerType(ERasterizerType::RT_CullFront);
	DecalMaterial->SetBlendStateType(EBlendStateType::BST_Decal);
	DecalMaterial->SetDepthStencilState(EDepthStencilStateType::DST_NO_TEST_NO_WRITE);
	DecalInfo.DecalMaterial = DecalMaterial;
	GEngine->GetWorld()->AddCurrentFrameDecalInfo(DecalInfo);
}
