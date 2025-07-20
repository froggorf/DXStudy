#include "CoreMinimal.h"
#include "ATestCube2.h"

#include "Engine/Components/UStaticMeshComponent.h"


ATestCube2::ATestCube2()
{
	CapsuleComponent = std::make_shared<UCapsuleComponent>();
	CapsuleComponent->SetRadius(75);
	CapsuleComponent->SetHalfHeight(75/2);
	CapsuleComponent->SetRelativeLocation({0,50,-1800});
	CapsuleComponent->SetupAttachment(GetRootComponent());

	TestComp = std::make_shared<UStaticMeshComponent>();
	AssetManager::GetAsyncAssetCache("SM_Box",[this](std::shared_ptr<UObject> Object)
		{
			TestComp->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
		});
	TestComp->SetupAttachment(CapsuleComponent);
	TestComp->SetRelativeScale3D({75,75,75});
	
	
}

void ATestCube2::BeginPlay()
{
	AActor::BeginPlay();
	TestComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	

	
	
}

void ATestCube2::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
}

void ATestCube2::TestCompFunc()
{
	static int a = 0;
	switch (a)
	{
	case 0:
		TestComp->SetTextureParam(0,0,UTexture::GetTextureCache("T_White"));
		a=1;
		break;
	case 1:
		TestComp->SetTextureParam(0,0,UTexture::GetTextureCache("T_Cube"));
		a=2;
		break;
	case 2:
		TestComp->SetTextureParam(0,0,UTexture::GetTextureCache("T_Box"));
		a=0;
		break;
	}
}
