#include "CoreMinimal.h"
#include "ASkyBox.h"

#include "Engine/UEngine.h"
#include "Engine/World/UWorld.h"


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
}

void ASkyBox::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
}

void ASkyBox::Tick_Editor(float DeltaSeconds)
{
	AActor::Tick_Editor(DeltaSeconds);
}

void ASkyBox::CalculateLocation()
{
	
}
