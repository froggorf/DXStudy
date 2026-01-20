#include "CoreMinimal.h"
#include "ACampfire.h"

#include "Engine/Class/Light/ULightComponent.h"
#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/Mesh/UStaticMesh.h"
#include "Engine/Niagara/UNiagaraSystem.h"

ACampfire::ACampfire()
{
	if (!GDirectXDevice)
	{
		return;
	}
	CampfireMesh = std::make_shared<UStaticMeshComponent>();
	CampfireMesh->SetupAttachment(GetRootComponent());
	CampfireMesh->SetRelativeScale3D(XMFLOAT3{30.0f, 30.0f, 30.0f});
	CampfireMesh->SetRelativeLocation(XMFLOAT3{0.0f,-65.0f,0.0f});

	FireVFX = std::make_shared<UNiagaraComponent>();
	FireVFX->SetupAttachment(GetRootComponent());
	const std::string NiagaraName = "NS_Campfire";
	auto System = UNiagaraSystem::GetNiagaraAsset(NiagaraName);
	if (!System)
	{
		const auto& AssetMap = AssetManager::GetAssetNameAndAssetPathMap();
		auto FoundPath = AssetMap.find(NiagaraName);
		if (FoundPath != AssetMap.end())
		{
			AssetManager::ReadMyAsset(FoundPath->second);
			System = UNiagaraSystem::GetNiagaraAsset(NiagaraName);
		}
	}
	if (System)
	{
		FireVFX->SetNiagaraAsset(System);
	}
	FireVFX->SetRelativeLocation(XMFLOAT3{0.0f,-30.0f,0.0f});

	FireLight = std::make_shared<ULightComponent>();
	FireLight->SetupAttachment(GetRootComponent());
	FireLight->SetLightType(ELightType::Point);
	FireLight->SetRadius(1500.0f);
	FireLight->SetLightColor({6.0f, 1.6f, 0.5f});
	FireLight->SetRelativeLocation({0.0f, 120.0f, 0.0f});
}

void ACampfire::Register()
{
	AActor::Register();

	AssetManager::GetAsyncAssetCache("SM_Campfire", [this](std::shared_ptr<UObject> Object)
		{
			CampfireMesh->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			CampfireMesh->SetCollisionObjectType(ECollisionChannel::WorldStatic);
		});

	FireVFX->Activate();
}
