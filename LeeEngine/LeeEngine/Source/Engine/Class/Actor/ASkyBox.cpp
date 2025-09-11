#include "CoreMinimal.h"
#include "ASkyBox.h"

#include "Engine/UEngine.h"
#include "Engine/World/UWorld.h"

/*
 Note: SkyBox 이미지 구하는법
 https://mstone8370.tistory.com/28
 텍스쳐 -> 스카이박스 -> .dds
 */

ASkyBox::ASkyBox()
{
	SM_SkyBox = std::make_shared<UStaticMeshComponent>();
	SM_SkyBox->SetupAttachment(GetRootComponent());
	std::shared_ptr<UMaterialInterface> SkyBoxMaterial = UMaterial::GetMaterialCache("M_SkyBox");
	if (SkyBoxMaterial)
	{
		SkyBoxMaterial->SetRasterizerType(ERasterizerType::RT_CullFront);
	}
	AssetManager::GetAsyncAssetCache("SM_SkyBox", [this](std::shared_ptr<UObject> LoadedStaticMesh)
	{
		SM_SkyBox->SetStaticMesh(std::static_pointer_cast<UStaticMesh>(LoadedStaticMesh));
	});
	//SM_SkyBox->SetRelativeScale3D({10000,10000,10000});
	SM_SkyBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SM_SkyBox->SetDoFrustumCulling(false);

	FScene::SetSkyBoxTexture_GameThread("T_Skybox_Forest");

	DirectionalLight= std::make_shared<ULightComponent>();
	DirectionalLight->SetupAttachment(GetRootComponent());
	DirectionalLight->SetWorldLocation(XMFLOAT3{-3000, 6000,-3000});
	DirectionalLight->SetLightType(ELightType::Directional);
	XMVECTOR RotQuat = XMQuaternionRotationRollPitchYaw(
		XMConvertToRadians(45.0f),
		XMConvertToRadians(45.0f),
		XMConvertToRadians(0.0f)
	);
	DirectionalLight->SetWorldRotation(RotQuat);
	constexpr float Radiance = 1.f;
	DirectionalLight->SetLightColor({Radiance,Radiance,Radiance});
}

void ASkyBox::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
}

void ASkyBox::Tick_Editor(float DeltaSeconds)
{
	AActor::Tick_Editor(DeltaSeconds);
}
