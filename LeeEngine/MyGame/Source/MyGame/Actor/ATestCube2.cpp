#include "CoreMinimal.h"
#include "ATestCube2.h"

#include "Engine/Components/UStaticMeshComponent.h"


ATestCube2::ATestCube2()
{
	{
		for (int i = 0 ; i < 20; ++i)
		{
			for (int j = 0; j < 20; ++j)
			{
				Ground[i][j]= std::make_shared<UStaticMeshComponent>();
				Ground[i][j]->SetupAttachment(GetRootComponent());
				Ground[i][j]->SetRelativeLocation(XMFLOAT3{-2500.0f + 250*i,-10,-2500.0f + 250 * j});
				Ground[i][j]->SetRelativeScale3D({125,20,125});	
			}
		}
		
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
}

void ATestCube2::Register()
{
	AActor::Register();

	for (int i = 0; i < 20; ++i)
	{
		for (int j = 0; j  < 20; ++j)
		{
			AssetManager::GetAsyncAssetCache("SM_Brick",[this,i,j](std::shared_ptr<UObject> Object)
				{
					Ground[i][j]->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
					Ground[i][j]->SetCollisionObjectType(ECollisionChannel::WorldStatic);
				});
		}
	}

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

	static bool a = false;
	if (!a)
	{
		if (ImGui::IsKeyDown(ImGuiKey_5))
		{
			a = true;

			GetWorld()->GetPersistentLevel()->DestroyActor(this);
		}
	}
}
