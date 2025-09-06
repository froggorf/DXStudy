#include "CoreMinimal.h"
#include "ATestCube2.h"

#include "Engine/Components/UStaticMeshComponent.h"


ATestCube2::ATestCube2()
{
	if (!GDirectXDevice) return;

	PBRTestComp = std::make_shared<UPBRTestComponent>();
	PBRTestComp->SetupAttachment(GetRootComponent());
	
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0;  j < 10; ++j)
		{
			PBRTestSphere[i][j] = std::make_shared<UStaticMeshComponent>();
			PBRTestSphere[i][j]->SetupAttachment(GetRootComponent());
			PBRTestSphere[i][j]->SetRelativeLocation({-500.0f + 100*i, 100.0f, -500.0f + 100 * j});
			AssetManager::GetAsyncAssetCache("SM_PBRTestSphere",[this, i,j](std::shared_ptr<UObject> Object)
				{
					PBRTestSphere[i][j]->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
					
				});
			constexpr float Size = 10.0f;
			PBRTestSphere[i][j]->SetRelativeScale3D({Size,Size,Size});
			
		}
	}
	
}

void ATestCube2::Register()
{
	AActor::Register();

	PBRTestComp->OnRChange.Add([this](float Value)
	{
			for (int i = 0; i < 10; ++i)
			{
				for (int j = 0; j < 10; ++j)
				{
					PBRTestSphere[i][j]->SetScalarParam(0,"Albedo_R", Value);
				}
			}	
	});
	PBRTestComp->OnBChange.Add([this](float Value)
		{
			for (int i = 0; i < 10; ++i)
			{
				for (int j = 0; j < 10; ++j)
				{
					PBRTestSphere[i][j]->SetScalarParam(0,"Albedo_B", Value);
				}
			}	
		});
	PBRTestComp->OnGChange.Add([this](float Value)
		{
			for (int i = 0; i < 10; ++i)
			{
				for (int j = 0; j < 10; ++j)
				{
					PBRTestSphere[i][j]->SetScalarParam(0,"Albedo_G", Value);
				}
			}	
		});
	PBRTestComp->OnButtonDelegate.Add([this]()
	{
			for (int i = 0; i < 10; ++i)
			{
				for (int j = 0; j < 10; ++j)
				{
					PBRTestSphere[i][j]->SetScalarParam(0,"Metallic", i * 0.1f);
					PBRTestSphere[i][j]->SetScalarParam(0,"Roughness", j * 0.1f);
				}
			}	
	});
	
}

void ATestCube2::BeginPlay()
{
	AActor::BeginPlay();
	

	
	
}

void ATestCube2::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
}
