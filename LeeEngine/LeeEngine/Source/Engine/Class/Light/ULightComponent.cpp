#include "CoreMinimal.h"
#include "ULightComponent.h"

#include "Engine/Physics/UCapsuleComponent.h"
#include "Engine/World/UWorld.h"


ULightComponent::ULightComponent()
{
	ShadowMRT = std::make_shared<FMultiRenderTarget>();
	ShadowCameraComp = std::make_shared<UCameraComponent>();
}

void ULightComponent::Register()
{
	USceneComponent::Register();

	Rename("LightComp" + std::to_string(ComponentID));
	AddLightInfo();	
}

void ULightComponent::SetLightType(ELightType Type)
{
	Info.LightType = static_cast<int>(Type);

	switch (Type)
	{
	case ELightType::Directional:
	{
		static UINT DirectionalIndex = 0;
		++DirectionalIndex;

		const std::string RTName = "DirShadowRT";
		const std::string DTName = "DirShadowDT";
		std::shared_ptr<UTexture> RenderTexture = AssetManager::CreateTexture(
																RTName
																,8192, 8192, DXGI_FORMAT_R32_FLOAT
																, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE
																, D3D11_USAGE_DEFAULT);
		
		std::shared_ptr<UTexture> DepthTexture =	AssetManager::CreateTexture(
																DTName
																,8192, 8192, DXGI_FORMAT_D32_FLOAT
																, D3D11_BIND_DEPTH_STENCIL
																, D3D11_USAGE_DEFAULT);
		
		ShadowMRT->Create(&RenderTexture, 1, DepthTexture);
		XMFLOAT4 ClearColor = {0,0,0,1};
		ShadowMRT->SetClearColor(&ClearColor,1);
		
		// 카메라 최대시야를 널널하게 설정
		ShadowCameraComp->SetFar(10000.f);
		ShadowCameraComp->SetProjectionType(EProjectionType::OrthoGraphic);
		ShadowCameraComp->SetOrthoScale(1.f);
		ShadowCameraComp->SetWidth(8192.f);
		ShadowCameraComp->SetAspectRatio(1.f);
	}
	break;
	case ELightType::Point:
	break;
	case ELightType::Spot:
	break;
	}
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
	ShadowCameraComp->SetWorldLocation(GetWorldLocation());
	const XMFLOAT4 Rot = GetWorldRotation();
	ShadowCameraComp->SetWorldRotation(XMLoadFloat4(&Rot));
	ShadowCameraComp->UpdateCameraMatrices();
	const XMMATRIX& LightView = ShadowCameraComp->GetViewMatrices().GetViewMatrix();
	const XMMATRIX& LightProj = ShadowCameraComp->GetViewMatrices().GetProjectionMatrix();
	Info.LightVP = LightView * LightProj;
	Info.ShadowMultiRenderTarget = ShadowMRT;
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
