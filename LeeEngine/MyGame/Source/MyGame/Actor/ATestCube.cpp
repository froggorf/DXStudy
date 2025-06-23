#include <CoreMinimal.h>

#include "ATestCube.h"

#include "Engine/UEditorEngine.h"
#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/Components/USceneComponent.h"
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/Mesh/UStaticMesh.h"

physx::PxDefaultAllocator gAllocator;
physx::PxDefaultErrorCallback gErrorCallback;
physx::PxFoundation* gFoundation = nullptr;
physx::PxPhysics* gPhysics = nullptr;
physx::PxScene* gScene = nullptr;
physx::PxRigidDynamic* sphereActor = nullptr;

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

	TestRigidSM = std::make_shared<UStaticMeshComponent>();
	AssetManager::GetAsyncAssetCache("SM_Sphere",[this](std::shared_ptr<UObject> Object)
		{
			TestRigidSM->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
		});
	TestRigidSM->SetupAttachment(GetRootComponent());
	TestRigidSM->SetRelativeLocation(XMFLOAT3{0.0f,-10.0f,50.0f});
	TestRigidSM->SetRelativeScale3D(XMFLOAT3{1.0f,1.0f,1.0f});

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

	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, physx::PxTolerancesScale());

	physx::PxSceneDesc SceneDesc(gPhysics->getTolerancesScale());
	SceneDesc.gravity = physx::PxVec3(0.0f,-9.8f*7.5, 0.0f);
	SceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	SceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(SceneDesc);

	// 바닥 (Static Plane)
	//physx::PxMaterial*    material = gPhysics->createMaterial(0.5f, 0.5f, 0.6f); // friction, restitution
	//physx::PxPlane        plane(physx::PxVec3(0, 1, 0), 50);
	//physx::PxTransform    planePose = PxTransformFromPlaneEquation(plane);
	//physx::PxRigidStatic* ground    = gPhysics->createRigidStatic(planePose);
	//// 3. Shape를 직접 생성해서 attach
	//physx::PxShape* groundShape = gPhysics->createShape(physx::PxPlaneGeometry(), *material);
	//ground->attachShape(*groundShape);
	//gScene->addActor(*ground);

	/*
	 *
	 // 1. 버텍스 데이터 준비
std::vector<PxVec3> vertices = { ... };

// 2. ConvexMeshDesc 세팅
PxConvexMeshDesc convexDesc;
convexDesc.points.count     = (PxU32)vertices.size();
convexDesc.points.stride    = sizeof(PxVec3);
convexDesc.points.data      = vertices.data();
convexDesc.flags            = PxConvexFlag::eCOMPUTE_CONVEX;

// 3. Cooking으로 convex mesh 생성
PxDefaultMemoryOutputStream buf;
cooking->cookConvexMesh(convexDesc, buf);
PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
PxConvexMesh* convexMesh = physics->createConvexMesh(input);

// 4. Shape 생성
PxConvexMeshGeometry convexGeom(convexMesh);
PxShape* shape = physics->createShape(convexGeom, *material);

// 5. Actor에 shape 부착
PxRigidDynamic* actor = physics->createRigidDynamic(pose);
actor->attachShape(*shape);
	 */
	physx::PxMaterial*    material = gPhysics->createMaterial(0.5f, 0.5f, 0.6f); // friction, restitution
	// convex hull 만들기
	{
		std::vector<physx::PxVec3> Vertices;
		const std::vector<std::vector<MyVertexData>>& VertexData = TestCube2->GetStaticMesh()->GetStaticMeshRenderData()->VertexData;
		size_t RequiredSize = 0;
		for (size_t i = 0; i < VertexData.size(); ++i)
		{
			RequiredSize += VertexData[i].size();
		}
		Vertices.reserve(RequiredSize);

		// 버텍스 정보 만들기
		for (const std::vector<MyVertexData>& VertexPerMesh : VertexData)
		{
			for (const MyVertexData& Vertex : VertexPerMesh)
			{
				Vertices.emplace_back(physx::PxVec3{Vertex.Pos.x,Vertex.Pos.y,Vertex.Pos.z});
			}
		}

		// Desc
		physx::PxConvexMeshDesc convexDesc;
		convexDesc.points.count  = static_cast<physx::PxU32>(Vertices.size());
		convexDesc.points.stride = sizeof(physx::PxVec3);
		convexDesc.points.data   = Vertices.data();
		convexDesc.flags         = physx::PxConvexFlag::eCOMPUTE_CONVEX;

		// Cooking
		physx::PxCookingParams params(gPhysics->getTolerancesScale());
		params.planeTolerance = 0.001f;
		params.areaTestEpsilon = 0.06f;
		params.gaussMapLimit = 32;

		physx::PxDefaultMemoryOutputStream buf;
		bool status = PxCookConvexMesh(params, convexDesc, buf);

		if (!status)
		{
			assert(nullptr && "잘못된 결과");
		};

		// 4. 생성된 convex mesh를 physics로 로드
		physx::PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
		physx::PxConvexMesh* convexMesh = gPhysics->createConvexMesh(input);


		// Shape
		physx::PxConvexMeshGeometry convexGeom(convexMesh, physx::PxMeshScale{physx::PxVec3{200,0.01,200}});
		physx::PxShape*      shape = gPhysics->createShape(convexGeom, *material);
		

		// RigidStatic
		physx::PxRigidStatic* Actor = gPhysics->createRigidStatic(physx::PxTransform{0,-50,0,{0,0,0,1}});
		Actor->attachShape(*shape);
		gScene->addActor(*Actor);
	}
	
	
	


	// 1. 구의 반지름
	float                  radius      = 1.0f;
	physx::PxVec3		   spherePos(0, -10, 0);
	sphereActor = gPhysics->createRigidDynamic(physx::PxTransform(spherePos));
	
	// 2. 구의 시작 위치 (y=10에서 떨어뜨리기)
	physx::PxShape* sphereShape = gPhysics->createShape(physx::PxSphereGeometry(radius), *material);
	sphereActor->attachShape(*sphereShape);
	sphereActor->setGlobalPose(physx::PxTransform{0.0f,-10.0f,-50.0f,{0.0f,0.0f,0.0f,1.0f}});

	physx::PxRigidBodyExt::updateMassAndInertia(*sphereActor, 1);
	gScene->addActor(*sphereActor);
	
	
}

void ATestCube::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	const float power = 300;
	if (ImGui::IsKeyDown(ImGuiKey_I))
	{
		sphereActor->addForce(physx::PxVec3{0,0,-1} * power);
	}
	if (ImGui::IsKeyDown(ImGuiKey_K))
	{
		sphereActor->addForce(physx::PxVec3{0,0,1} * power);
	}
	if (ImGui::IsKeyDown(ImGuiKey_J))
	{
		sphereActor->addForce(physx::PxVec3{-1,0,0} * power);
	}
	if (ImGui::IsKeyDown(ImGuiKey_L))
	{
		sphereActor->addForce(physx::PxVec3{1,0,0} * power);
	}

	gScene->simulate(DeltaSeconds); // 예: deltaTime = 1/60.0f
	gScene->fetchResults(true);

	physx::PxTransform pos = sphereActor->getGlobalPose();
	TestRigidSM->SetRelativeLocation(XMFLOAT3{pos.p.x,pos.p.y,-pos.p.z}); 

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
