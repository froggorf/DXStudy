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
		Decal->SetRelativeScale3D({500,10,500});

	}

	{
		PizzaDecal1 =std::make_shared<UDecalComponent>();
		const std::shared_ptr<UMaterialInterface>& DecalMaterial = UMaterial::GetMaterialCache("MI_SkillRangeDecal");
		PizzaDecal1->SetupAttachment(GetRootComponent());
		PizzaDecal1->SetDecalMaterial(DecalMaterial);
		PizzaDecal1->SetIsLight(true);
		PizzaDecal1->SetRelativeScale3D({500,10,500});
		PizzaDecal1->SetRelativeLocation({0,0,-500});

	}
	{
		PizzaDecal2 =std::make_shared<UDecalComponent>();
		const std::shared_ptr<UMaterialInterface>& DecalMaterial = UMaterial::GetMaterialCache("MI_SkillRangeDecal");
		PizzaDecal2->SetupAttachment(GetRootComponent());
		PizzaDecal2->SetDecalMaterial(DecalMaterial);
		PizzaDecal2->SetIsLight(true);
		PizzaDecal2->SetRelativeScale3D({500,10,500});
		PizzaDecal2->SetRelativeLocation({0,0,-1000});

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

	PizzaDecal1->GetDecalMaterial()->SetScalarParam("HalfAngleDeg", 30.0f);
	PizzaDecal2->GetDecalMaterial()->SetScalarParam("HalfAngleDeg", 60.0f);
}

void ATestCube2::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	static float Value = 0.0f;
	Value += DeltaSeconds;
	
	if (Value >= 1.0f)
	{
		Value = 0.0f;
	}

	Decal->GetDecalMaterial()->SetScalarParam("Progress", Value);

	PizzaDecal1->GetDecalMaterial()->SetScalarParam("Progress", Value);

	static float Angle = 0.0f;
	Angle = fmod(Angle + DeltaSeconds * 30, 360.0f);
	
	static XMFLOAT3 Forward = {0,0,1};
	XMFLOAT3 Rotate;
	XMStoreFloat3(&Rotate, XMVector3Rotate(XMLoadFloat3(&Forward), XMQuaternionRotationRollPitchYaw(0, XMConvertToRadians(Angle), 0)));
	PizzaDecal2->GetDecalMaterial()->SetScalarParam("ForwardX", Rotate.x);
	PizzaDecal2->GetDecalMaterial()->SetScalarParam("ForwardZ", Rotate.z);
	PizzaDecal2->GetDecalMaterial()->SetScalarParam("Progress", Value);
}
