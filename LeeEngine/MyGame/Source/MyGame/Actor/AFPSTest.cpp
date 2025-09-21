#include "CoreMinimal.h"
#include "AFPSTest.h"

AFPSTest::AFPSTest()
{
	constexpr float Size = 5000.0f;
	SM_Box = std::make_shared<UStaticMeshComponent>();
	SM_Box->SetupAttachment(GetRootComponent());
	SM_Box->SetRelativeScale3D({Size,2.0,Size});


	//AssetManager::GetAsyncAssetCache("SM_Cube",[this](std::shared_ptr<UObject> Object)
	AssetManager::GetAsyncAssetCache("SM_DeferredCube",[this](std::shared_ptr<UObject> Object)
		{
			SM_Box->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
		});
	SM_Box->SetWorldLocation({0,0,0});

	
	constexpr int Count = 20;
	int Gap = Size / ( Count/2);

	for (int i = 0; i < Count; ++i)
	{
		for (int j  = 0; j < Count; ++j)
		{
			Light[i][j] = std::make_shared<ULightComponent>();
			float R = static_cast<float>(rand()) / RAND_MAX;
			float G = static_cast<float>(rand()) / RAND_MAX;
			float B = static_cast<float>(rand()) / RAND_MAX;
			Light[i][j]->SetupAttachment(GetRootComponent());
			Light[i][j]->SetLightColor({R*2,G*2,B*2});
			Light[i][j]->SetLightType(ELightType::Point);
			Light[i][j]->SetWorldLocation({ -Size + Gap * i , 50.0f, -Size+ Gap*j});
			Light[i][j]->SetRadius(1000);
		}
	}
}
