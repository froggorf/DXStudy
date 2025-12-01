#include "CoreMinimal.h"
#include "ATestCube2.h"

#include "Engine/Class/Light/ULightComponent.h"
#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/Components/UStaticMeshComponent.h"


ATestCube2::ATestCube2()
{
	{
		Ground= std::make_shared<UStaticMeshComponent>();
		Ground->SetupAttachment(GetRootComponent());
		Ground->SetRelativeScale3D({2500.0f,1.0f,2500.0f});	
		
	}

	{
		SM_Chair = std::make_shared<UStaticMeshComponent>();
		SM_Chair->SetupAttachment(GetRootComponent());
		SM_Chair->SetRelativeLocation({-300,10,0});
	}

	{
		SM_Couch = std::make_shared<UStaticMeshComponent>();
		SM_Couch->SetupAttachment(GetRootComponent());
		SM_Couch->SetRelativeLocation({-300,10,-250});
		SM_Couch->SetIsMonochromeObject(false);
	}

	{
		Decal =std::make_shared<UDecalComponent>();
		const std::shared_ptr<UMaterialInterface>& DecalMaterial = UMaterial::GetMaterialCache("MI_SkillRangeDecal");
		Decal->SetupAttachment(GetRootComponent());
		Decal->SetDecalMaterial(DecalMaterial);
		Decal->SetIsLight(true);
		Decal->SetRelativeScale3D({300,10,300.0f});

	}
}

void ATestCube2::Register()
{
	AActor::Register();

	AssetManager::GetAsyncAssetCache("SM_Brick",[this](std::shared_ptr<UObject> Object)
		{
			Ground->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			Ground->SetCollisionObjectType(ECollisionChannel::WorldStatic);
		});

	AssetManager::GetAsyncAssetCache("SM_UE_Couch",[this](std::shared_ptr<UObject> Object)
		{
			SM_Couch->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			SM_Couch->SetCollisionObjectType(ECollisionChannel::WorldStatic);
		});


	AssetManager::GetAsyncAssetCache("SM_UE_Chair",[this](std::shared_ptr<UObject> Object)
		{
			SM_Chair->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			SM_Chair->SetCollisionObjectType(ECollisionChannel::WorldStatic);
		});
}

void ATestCube2::BeginPlay()
{
	AActor::BeginPlay();

}

void ATestCube2::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	static bool bFill = true;
	static float Value = 0.0f;
	if (bFill)
	{
		Value += DeltaSeconds;
	}
	else
	{
		Value -= DeltaSeconds;
	}
	
	if (Value >= 1.0f)
	{
		bFill = !bFill;
		Value = 1.0f;
	}
	else if (Value <= 0.0f)
	{
		bFill = !bFill;
		Value = 0.0f;
	}

	Decal->GetDecalMaterial()->SetScalarParam("Progress", Value);
}
