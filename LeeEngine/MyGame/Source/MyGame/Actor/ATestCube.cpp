#include <CoreMinimal.h>

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
	TestCube2->SetCollisionObjectType(ECollisionChannel::WorldStatic);
	

	TestCubeSM1	 = std::make_shared<UStaticMeshComponent>();
	TestCubeSM2	 = std::make_shared<UStaticMeshComponent>();
	TestCubeSM3	 = std::make_shared<UStaticMeshComponent>();
	TestCubeSM4	 = std::make_shared<UStaticMeshComponent>();
	TestCubeSM5	 = std::make_shared<UStaticMeshComponent>();
	TestCubeSM6	 = std::make_shared<UStaticMeshComponent>();
	TestCubeSM9	 = std::make_shared<UStaticMeshComponent>();
	TestCubeSM7	 = std::make_shared<UStaticMeshComponent>();
	TestCubeSM8	 = std::make_shared<UStaticMeshComponent>();
	TestCubeSM10 = std::make_shared<UStaticMeshComponent>();
	TestCubeSM11 = std::make_shared<UStaticMeshComponent>();
	TestCubeSM12 = std::make_shared<UStaticMeshComponent>();
	TestCubeSM13 = std::make_shared<UStaticMeshComponent>();
	TestCubeSM14 = std::make_shared<UStaticMeshComponent>();
	TestCubeSM15 = std::make_shared<UStaticMeshComponent>();
	TestCubeSM16 = std::make_shared<UStaticMeshComponent>();
	TestCubeSM17 = std::make_shared<UStaticMeshComponent>();
	TestCubeSM18 = std::make_shared<UStaticMeshComponent>();
	TestCubeSM19 = std::make_shared<UStaticMeshComponent>();
	TestCubeSM20 = std::make_shared<UStaticMeshComponent>();
	TestCubeSM21 = std::make_shared<UStaticMeshComponent>();


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
		
			SMVec[index]->SetCollisionEnabled(ECollisionEnabled::Physics);
			SMVec[index]->SetCollisionObjectType(ECollisionChannel::WorldDynamic);
			SMVec[index]->SetCollisionResponseToChannel(ECollisionChannel::WorldStatic,ECollisionResponse::Block);
			SMVec[index]->SetCollisionResponseToChannel(ECollisionChannel::WorldDynamic,ECollisionResponse::Block);
			AssetManager::GetAsyncAssetCache("SM_Box",[SMVec,index](std::shared_ptr<UObject> Object)
				{
					SMVec[index]->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
				});

			SMVec[index]->SetupAttachment(GetRootComponent());
			SMVec[index]->SetRelativeScale3D(XMFLOAT3{5.0f,5.0f,5.0f});
			SMVec[index]->SetRelativeLocation(XMFLOAT3{x,y+50,0});
			//SMVec[index]->SetRelativeLocation(XMFLOAT3{0,50,0});
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

	TriggerBox1 = std::make_shared<UBoxComponent>();
	TriggerBox1->SetExtent(XMFLOAT3{50,3,50});
	TriggerBox1->SetupAttachment(GetRootComponent());
	TriggerBox1->SetRelativeLocation(XMFLOAT3{0,-10,0});
}

void ATestCube::BeginPlay()
{
	AActor::BeginPlay();

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

	TestCube2->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	for (const std::shared_ptr<UStaticMeshComponent>& SMC : SMVec)
	{
		SMC->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::Physics);
		SMC->GetBodyInstance()->OnComponentHit.Add(this, &ATestCube::OnComponentHitEvent);
		SMC->GetBodyInstance()->OnComponentBeginOverlap.Add(this, &ATestCube::OnComponentBeginOverlapEvent);
	}
}

void ATestCube::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
	auto CalculateForward = [](XMFLOAT3& Start, XMFLOAT3& End)
	{
		XMMATRIX InvMat = XMMatrixInverse(nullptr, GEngine->Test_DeleteLater_GetViewMatrix());
		XMFLOAT3 ForwardVector;
		float Distance = 100.0f;
		ForwardVector.x = InvMat.r[2].m128_f32[0]*Distance;
		ForwardVector.y = InvMat.r[2].m128_f32[1]*Distance;
		ForwardVector.z = InvMat.r[2].m128_f32[2]*Distance;
		Start.x = InvMat.r[3].m128_f32[0];
		Start.y = InvMat.r[3].m128_f32[1];
		Start.z = InvMat.r[3].m128_f32[2];

		End.x = Start.x + ForwardVector.x;
		End.y = Start.y + ForwardVector.y;
		End.z = Start.z + ForwardVector.z;
			
	};

	static float LastLineTraceRenderThreadTime = -1;
	XMFLOAT3 Start,End;
	CalculateForward(Start,End);

	if (ImGui::IsKeyReleased(ImGuiKey_3) && LastLineTraceRenderThreadTime != RenderingThreadFrameCount)
	{
		LastLineTraceRenderThreadTime = RenderingThreadFrameCount;

		FHitResult HitResult;
		if (gPhysicsEngine->LineTraceSingleByChannel(Start,End, {ECollisionChannel::WorldStatic},  HitResult, 10.0f))
		{
			MY_LOG("LineTrace",EDebugLogLevel::DLL_Warning,"WorldStatic");
		}
		
	}
	if (ImGui::IsKeyReleased(ImGuiKey_4) && LastLineTraceRenderThreadTime != RenderingThreadFrameCount)
	{
		LastLineTraceRenderThreadTime = RenderingThreadFrameCount;

		FHitResult HitResult;
		if (gPhysicsEngine->LineTraceSingleByChannel(Start,End, {ECollisionChannel::WorldDynamic}, HitResult,
			10.0f,{1.0f,1.0f,0.0f}, {0.0f,0.0f,1.0f}))
		{
			if (UStaticMeshComponent* SMComp = dynamic_cast<UStaticMeshComponent*>(HitResult.HitComponent->GetAttachParent().get()))
			{
				SMComp->SetTextureParam(0,0, UTexture::GetTextureCache("T_Box"));
			}
		}
	}

	const float power = 3;
	if (ImGui::IsKeyDown(ImGuiKey_I))
	{
		TestCubeSM1->AddForce({0,0,power});
	}
	if (ImGui::IsKeyDown(ImGuiKey_K))
	{
		TestCubeSM1->AddForce({0,0,-power});
	}
	if (ImGui::IsKeyDown(ImGuiKey_J))
	{
		TestCubeSM1->AddForce({-power,0,0});
	}
	if (ImGui::IsKeyDown(ImGuiKey_L))
	{
		TestCubeSM1->AddForce({power,0,0});
	}

	if (ImGui::IsKeyPressed(ImGuiKey_5))
	{
		for (UINT i = 0; i < static_cast<UINT>(ECollisionChannel::Count); ++i)
		{
			TestCubeSM1->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(i), ECollisionResponse::Block);	
		}
	}

	if (ImGui::IsKeyPressed(ImGuiKey_6))
	{
		for (UINT i = 0; i < static_cast<UINT>(ECollisionChannel::Count); ++i)
		{
			TestCubeSM1->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(i), ECollisionResponse::Overlap);	
		}
	}

	if (ImGui::IsKeyPressed(ImGuiKey_7))
	{
		TestCubeSM1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (ImGui::IsKeyPressed(ImGuiKey_8))
	{
		TestCubeSM1->SetCollisionEnabled(ECollisionEnabled::Physics);
	}

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

void ATestCube::OnComponentHitEvent(UShapeComponent* HitComponent, AActor* OtherActor, UShapeComponent* OtherComp, const FHitResult& HitResults)
{
	UStaticMeshComponent* StaticMeshComp =  static_cast<UStaticMeshComponent*>(HitComponent->GetAttachParent().get());
	StaticMeshComp->SetTextureParam(0, 0, UTexture::GetTextureCache("T_White"));
}

void ATestCube::OnComponentBeginOverlapEvent(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp)
{
	UStaticMeshComponent* StaticMeshComp =  static_cast<UStaticMeshComponent*>(OverlappedComponent->GetAttachParent().get());
	StaticMeshComp->SetTextureParam(0, 0, UTexture::GetTextureCache("T_Cube"));
}
