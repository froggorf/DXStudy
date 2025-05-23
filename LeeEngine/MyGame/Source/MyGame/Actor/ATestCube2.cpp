#include "CoreMinimal.h"
#include "ATestCube2.h"

#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/Mesh/UStaticMesh.h"

ATestCube2::ATestCube2()
{
	std::vector<std::shared_ptr<UStaticMeshComponent>> Comps = {
		SM0,
		SM1,
		SM2,
		SM3,
		SM4,
		SM5,
		SM6,
		SM7,
		SM8,
		SM9,
		SM10,
		SM11,
		SM12,
		SM13,
		SM14,
		SM15,
		SM16,
		SM17,
		SM18,
		SM19,
		SM20,
		SM21,
		SM22,
		SM23,
		SM24,
		SM25,
		SM26,
		SM27,
		SM28,
		SM29,
		SM30,
		SM31,
		SM32,
		SM33,
		SM34,
		SM35,
		SM36,
		SM37,
		SM38,
		SM39,
		SM40,
		SM41,
		SM42,
		SM43,
		SM44,
		SM45,
		SM46,
		SM47,
		SM48,
		SM49
	};
	

	for(size_t i = 0; i < Comps.size(); ++i)
	{
		Comps[i] = std::make_shared<UStaticMeshComponent>();
		Comps[i]->SetupAttachment(GetRootComponent());
		// 프리로드 데이터를 활용하는 방법
		//Comps[i]->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Cube"));
		// 비동기 로드
		std::shared_ptr<UStaticMeshComponent> Target = Comps[i];
		std::string Name = "SM_AsyncTest" + std::to_string(i%16);
		AssetManager::GetAsyncAssetCache(Name, [Target,i](std::shared_ptr<UObject> Object)
		{
			Target->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
		});
		Target->SetRelativeScale3D(XMFLOAT3(1.0f,1.0f,1.0f));
		Target->SetRelativeLocation(XMFLOAT3(100.0f*(i/5), 0.0f, 100*(i%5)));
	
	}

	
}

void ATestCube2::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
}
