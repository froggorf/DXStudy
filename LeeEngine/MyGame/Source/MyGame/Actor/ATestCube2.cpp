#include "CoreMinimal.h"
#include "ATestCube2.h"

#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/Mesh/UStaticMesh.h"

ATestCube2::ATestCube2()
{
	UTestComponent* NewTestComp = dynamic_cast<UTestComponent*>(CreateDefaultSubobject("TestActorComp", "UTestComponent"));
	if (NewTestComp)
	{
		TestComp = std::make_shared<UTestComponent>(*NewTestComp);
	}
	
	std::vector<std::shared_ptr<USkeletalMeshComponent>> Comps = {
		SK0,
		SK1,
		SK2,
		SK3,
		SK4,
		SK5,
		SK6,
		SK7,
		SK8,
		SK9,
		SK10,
		SK11,
		SK12,
		SK13,
		SK14
	};
	

	for(size_t i = 0; i < Comps.size(); ++i)
	{
		Comps[i] = std::make_shared<USkeletalMeshComponent>();
		Comps[i]->SetupAttachment(GetRootComponent());
		// 프리로드 데이터를 활용하는 방법
		//Comps[i]->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Cube"));
		// 비동기 로드
		std::shared_ptr<USkeletalMeshComponent> Target = Comps[i];
		std::string Name = "SK_AsyncTest" + std::to_string(i);
		AssetManager::GetAsyncAssetCache(Name, [Target,i](std::shared_ptr<UObject> Object)
		{
			Target->SetSkeletalMesh(std::dynamic_pointer_cast<USkeletalMesh>(Object));
		});
		Target->SetAnimInstanceClass("UMyAnimInstance");
		
		Target->SetRelativeScale3D(XMFLOAT3(1.0f,1.0f,1.0f));
		Target->SetRelativeLocation(XMFLOAT3(100.0f*(i/5), 0.0f, 100*(i%5)));
	}

	
}

void ATestCube2::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
}
