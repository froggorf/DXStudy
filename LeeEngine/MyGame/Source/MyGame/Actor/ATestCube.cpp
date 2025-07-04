﻿#include <CoreMinimal.h>

#include "ATestCube.h"

#include "Engine/UEditorEngine.h"
#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/Components/USceneComponent.h"
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/Mesh/UStaticMesh.h"
#include "Engine/Physics/UPhysicsEngine.h"

physx::PxRigidDynamic* SphereActor = nullptr;
physx::PxShape* SphereShape;
ATestCube::ATestCube()
{
	//GetRootComponent()->SetRelativeRotation(XMFLOAT3(0.0f,0.0f,0.0f));
	//TestCubeStaticMeshComp = std::make_shared<UStaticMeshComponent>();
	//TestCubeStaticMeshComp->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Point"));
	//TestCubeStaticMeshComp->SetupAttachment(GetRootComponent());
	//TestCubeStaticMeshComp->SetRelativeScale3D(XMFLOAT3{30.0f,30.0f,30.0f});
	//
	//TestCubeStaticMeshComp->SetRelativeLocation(XMFLOAT3(0.0f,0.0f,0.0f));
	////TestCubeStaticMeshComp->SetRelativeRotation(XMFLOAT3(0.0f,0.0f,0.0f));
	//TestCubeStaticMeshComp->SetRelativeRotation(XMFLOAT3(0.0f,0.0f,0.0f));
	//TestCubeStaticMeshComp->SetRelativeScale3D(XMFLOAT3(0.2f,0.2f,0.2f));
	//
	TestCube2 = std::make_shared<UStaticMeshComponent>();
	TestCube2->SetupAttachment(GetRootComponent());
	// 비동기 에셋 로드 테스트
	AssetManager::GetAsyncAssetCache("SM_Cube",[this](std::shared_ptr<UObject> Object)
	{
		TestCube2->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
	});

	//TestCube2->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Cube"));
	TestCube2->SetRelativeScale3D(XMFLOAT3(200.0f, 0.01f, 200.0f));
	TestCube2->SetRelativeLocation(XMFLOAT3(0.0f, -50, 0.0f));

	
	
	

	std::vector<std::shared_ptr<UStaticMeshComponent>> SMVec = {
		TestCubeSM1	,
		TestCubeSM2	,
		TestCubeSM3	,
		TestCubeSM4	,
		TestCubeSM5	,
		TestCubeSM6	,
		TestCubeSM9	,
		TestCubeSM7	,
		TestCubeSM8	,
		TestCubeSM10,
		TestCubeSM11,
		TestCubeSM12,
		TestCubeSM13,
		TestCubeSM14,
		TestCubeSM15,
		TestCubeSM16,
		TestCubeSM17,
		TestCubeSM18,
		TestCubeSM19,
		TestCubeSM20,
		TestCubeSM21,
	};

	// 피라미드의 맨 위(0,0)에서 시작
	const float XOffset = 15.0f;
	const float YOffset = 15.0f;

	int index = 0;
	int row = 1;

	while (index < SMVec.size())
	{
		

		// 각 줄의 시작 위치를 중앙 정렬하려면
		float startX = -((row - 1) * XOffset) / 2.0f;
		float y = (1-row) * YOffset;

		for (int col = 0; col < row && index < SMVec.size(); ++col, ++index)
		{
			float x = startX + col * XOffset;
			SMVec[index]= std::make_shared<UStaticMeshComponent>();
			SMVec[index]->SetCollisionEnabled(ECollisionType::Dynamic);
			AssetManager::GetAsyncAssetCache("SM_Box",[SMVec,index](std::shared_ptr<UObject> Object)
				{
					SMVec[index]->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
				});

			SMVec[index]->SetupAttachment(GetRootComponent());
			SMVec[index]->SetRelativeScale3D(XMFLOAT3{5.0f,5.0f,5.0f});
			SMVec[index]->SetRelativeLocation(XMFLOAT3{x,y+50,0});
		}
		++row;
	}

	DummyComp = std::make_shared<USceneComponent>();
	DummyComp->Rename("DummyComp");
	DummyComp->SetupAttachment(GetRootComponent());
	DummyComp->SetRelativeLocation(XMFLOAT3{-45.0f, 50.0f, 0.0f});

	NC_BillboardRibbon = std::make_shared<UNiagaraComponent>();
	NC_BillboardRibbon->SetupAttachment(DummyComp);
	std::shared_ptr<UNiagaraSystem> System = UNiagaraSystem::GetNiagaraAsset("NS_Ribbon");
	NC_BillboardRibbon->SetNiagaraAsset(System);
	NC_BillboardRibbon->SetRelativeLocation(XMFLOAT3{0.0, 30.0f, 0.0f});
	NC_BillboardRibbon->SetRelativeRotation(XMFLOAT3{90.0f, 0.0f, 0.0f});

	DummyComp2 = std::make_shared<USceneComponent>();
	DummyComp2->Rename("DummyComp2");
	DummyComp2->SetupAttachment(GetRootComponent());
	DummyComp2->SetRelativeLocation(XMFLOAT3{45.0f, 50.0f, 0.0f});

	NC_Ribbon = std::make_shared<UNiagaraComponent>();
	NC_Ribbon->SetupAttachment(DummyComp2);
	std::shared_ptr<UNiagaraSystem> System2 = UNiagaraSystem::GetNiagaraAsset("NS_RibbonX");
	NC_Ribbon->SetNiagaraAsset(System2);
	NC_Ribbon->SetRelativeLocation(XMFLOAT3{0.0, 30.0f, 0.0f});
	NC_Ribbon->SetRelativeRotation(XMFLOAT3{0.0f, 0.0f, 0.0f});

	NC_Ribbon = std::make_shared<UNiagaraComponent>();
	NC_Ribbon->SetupAttachment(GetRootComponent());

	NC_Ribbon->SetNiagaraAsset(UNiagaraSystem::GetNiagaraAsset("NS_Test"));
	NC_Ribbon->SetRelativeLocation(XMFLOAT3{-50.0, -15.0f, 0.0f});
	//NC_Ribbon->SetRelativeLocation(XMFLOAT3{-50.0, -50.0f, 100.0f});
	NC_Ribbon->SetRelativeRotation(XMFLOAT3{0.0f, 0.0f, 0.0f});

	Rename("ATestCube" + std::to_string(ActorID));
}

void ATestCube::BeginPlay()
{
	AActor::BeginPlay();
}

void ATestCube::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	//const float power = 3;
	//if (ImGui::IsKeyDown(ImGuiKey_I))
	//{
	//	SphereActor->addForce(physx::PxVec3{0,0,-1} * power);
	//}
	//if (ImGui::IsKeyDown(ImGuiKey_K))
	//{
	//	SphereActor->addForce(physx::PxVec3{0,0,1} * power);
	//}
	//if (ImGui::IsKeyDown(ImGuiKey_J))
	//{
	//	SphereActor->addForce(physx::PxVec3{-1,0,0} * power);
	//}
	//if (ImGui::IsKeyDown(ImGuiKey_L))
	//{
	//	SphereActor->addForce(physx::PxVec3{1,0,0} * power);
	//}
	//
	//gPhysicsEngine->TickPhysics(DeltaSeconds);

	//physx::PxTransform pos = SphereActor->getGlobalPose();
	//TestRigidSM->SetRelativeLocation(XMFLOAT3{pos.p.x,pos.p.y,-pos.p.z}); 

	//SetActorRotation(XMFLOAT4(0.0f,0.0f,0.0f,1.0f));

	DummyComp2->AddWorldRotation(XMFLOAT3(0.0f, 0.0f, DeltaSeconds * 90));
	//NiagaraComp2->AddWorldRotation(XMFLOAT3(0.0f,.0f*DeltaSeconds,0.0f));
	if (false)
	{
		XMFLOAT4 RR    = DummyComp->GetRelativeRotation();
		XMVECTOR RRVec = XMLoadFloat4(&RR);

		XMVECTOR Z1Quat = XMQuaternionRotationAxis(XMVectorSet(0, 0, 1, 0), DeltaSeconds);

		DummyComp->SetRelativeRotation(XMQuaternionMultiply(RRVec, Z1Quat));
	}
	static XMVECTOR A{-100, 30, -100};
	static XMVECTOR B{-100, 30, -50};
	static XMVECTOR C{100, 30, 100};
	static float    t = 0.0f;
	t += DeltaSeconds;
	XMVECTOR Target;
	if (t < 1.0f)
	{
		Target = XMVectorLerp(A, B, t);
	}
	else if (t < 2.0f)
	{
		Target = XMVectorLerp(B, A, t - 1);
	}
	else
	{
		Target = XMVectorLerp(B, A, t - 1);
		t -= 2.0f;
	}
	XMFLOAT3 Loc;
	XMStoreFloat3(&Loc, Target);
	DummyComp->SetWorldLocation(Loc);


	static bool bCheck = false;
	if(ImGui::IsKeyReleased(ImGuiKey_P))
	{
		if(!bCheck)
		{
			TestCube2->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Sphere"));
			MY_LOG("PRESSED", EDebugLogLevel::DLL_Warning, "P");
			bCheck = true;
		}
		
	}
	{
		static bool bCheck2 = false;
		if(ImGui::IsKeyReleased(ImGuiKey_O))
		{
			if(!bCheck2)
			{
				TestCube2->SetStaticMesh(UStaticMesh::GetStaticMesh("SM_Cube"));
				MY_LOG("PRESSED", EDebugLogLevel::DLL_Warning, "O");
				bCheck2 = true;
			}

		}
	}
}
