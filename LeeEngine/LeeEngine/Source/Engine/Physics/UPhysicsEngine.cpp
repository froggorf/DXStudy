#include "CoreMinimal.h"
#include "UPhysicsEngine.h"

#include "UShapeComponent.h"

std::unique_ptr<UPhysicsEngine> gPhysicsEngine = nullptr;

UPhysicsEngine::UPhysicsEngine()
{
	PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	PxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *PxFoundation, physx::PxTolerancesScale());
	

	physx::PxSceneDesc SceneDesc(PxPhysics->getTolerancesScale());
	SceneDesc.gravity = physx::PxVec3(0.0f,-9.8f*7.5f, 0.0f);
	SceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	SceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
	PxScene = PxPhysics->createScene(SceneDesc);

	DefaultMaterial = PxPhysics->createMaterial(0.5f, 0.5f, 0.6f); // friction, restitution

	
}

UPhysicsEngine::~UPhysicsEngine()
{
	if (DefaultMaterial)
	{
		DefaultMaterial->release();	
	}
	
	if (PxScene)
	{
		PxScene->release();	
	}
	
	if (PxPhysics)
	{
		PxPhysics->release();	
	}
	
	if (PxFoundation)
	{
		PxFoundation->release();	
	}
	
}

void UPhysicsEngine::TickPhysics(float DeltaSeconds) const
{
	static float CurFrameTime = 0.0f;
	CurFrameTime += DeltaSeconds;

	static float UpdatePerSecond = 60;
	static float UpdateTime = 1.0f / UpdatePerSecond;
	if (CurFrameTime >= UpdateTime)
	{
		if (PxScene)
		{
			PxScene->simulate(UpdateTime);
			PxScene->fetchResults(true);
		}
		CurFrameTime -= UpdateTime;

		// 위치 정보도 적용해주기
		physx::PxU32 ActorNum = PxScene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC);
		std::vector<physx::PxActor*> SceneActors(ActorNum);
		PxScene->getActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC, SceneActors.data(), ActorNum);

		for (physx::PxU32 i = 0; i < ActorNum; ++i)
		{
			physx::PxActor* Actor = SceneActors[i];

			if (UShapeComponent* ShapeComp = static_cast<UShapeComponent*>(Actor->userData))
			{
				if (physx::PxRigidDynamic* RigidDynamic = Actor->is<physx::PxRigidDynamic>())
				{
					const physx::PxTransform& PxTransform = RigidDynamic->getGlobalPose();
					FTransform Transform{{PxTransform.p.x, PxTransform.p.y, -PxTransform.p.z},{PxTransform.q.x, PxTransform.q.y, PxTransform.q.z, PxTransform.q.w},{1, 1, 1}};
					ShapeComp->SetWorldTransform(Transform);
				}
			}
		}
	}

	
}

physx::PxShape* UPhysicsEngine::CreateSphereShape(const float Radius) const
{
	return PxPhysics->createShape(physx::PxSphereGeometry(Radius), *DefaultMaterial);
}

physx::PxRigidActor* UPhysicsEngine::CreateAndRegisterActor(const FTransform& Transform, physx::PxShape* InShape, const float Mass, bool bIsDynamic) const
{
	physx::PxTransform ActorTransform {Transform.Translation.x,Transform.Translation.y,-Transform.Translation.z, {0.0f,0.0f,0.0f,1.0f}};
	physx::PxRigidActor* Actor = nullptr;
	if (bIsDynamic)
	{
		physx::PxRigidDynamic* DynamicActor = PxPhysics->createRigidDynamic(ActorTransform);
		
		physx::PxRigidBodyExt::updateMassAndInertia(*DynamicActor, Mass);
		Actor = DynamicActor;
		
	}
	else
	{
		Actor = PxPhysics->createRigidStatic(ActorTransform);
	}

	Actor->attachShape(*InShape);
	PxScene->addActor(*Actor);

	return Actor;
}


physx::PxRigidActor* UPhysicsEngine::CreateAndRegisterConvexActor(const FTransform& Transform, const std::shared_ptr<UStaticMesh>& StaticMesh, const float Mass, Microsoft::WRL::ComPtr<ID3D11Buffer>& OutVertexBuffer, bool bIsDynamic) const
{
	physx::PxConvexMesh* ConvexMesh = CreateConvexMesh(StaticMesh);
	if (!ConvexMesh)
	{
		assert(nullptr && "에러 - CreateConvexMesh");
	}

#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)

	OutVertexBuffer = CreateVertexBufferForConvexActor(ConvexMesh);
#endif



	// Shape
	float ScaleOffset = 1.f;
	physx::PxMeshScale Scale = physx::PxVec3{Transform.Scale3D.x * ScaleOffset,Transform.Scale3D.y * ScaleOffset,Transform.Scale3D.z * ScaleOffset};
	physx::PxConvexMeshGeometry convexGeom(ConvexMesh, Scale);
	physx::PxShape*      shape = PxPhysics->createShape(convexGeom, *DefaultMaterial);
	shape->setContactOffset(0.009f);
	shape->setRestOffset(0.0f);

	// RigidActor
	physx::PxRigidActor* Actor = nullptr;
	if (bIsDynamic)
	{
		physx::PxRigidDynamic* DynamicActor = PxPhysics->createRigidDynamic(physx::PxTransform{{Transform.Translation.x,Transform.Translation.y,-Transform.Translation.z},{0,0,0,1}});
		physx::PxRigidBodyExt::updateMassAndInertia(*DynamicActor, Mass);
		Actor = DynamicActor;
	}
		
	else
	{
		Actor = PxPhysics->createRigidStatic(physx::PxTransform{{Transform.Translation.x,Transform.Translation.y,-Transform.Translation.z},{0,0,0,1}});;
	}
								
	Actor->attachShape(*shape);
	PxScene->addActor(*Actor);

	return Actor;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> UPhysicsEngine::CreateVertexBufferForConvexActor(const physx::PxConvexMesh* ConvexMesh)
{
	if (!ConvexMesh)
	{
		return nullptr;
	}

	// ConvexMesh 만들어진 것을
	// 디버그 드로우 용도로 쓰기 위해
	// VertexBuffer와 IndexBuffer 생성
	
	const physx::PxU32   VertexCount = ConvexMesh->getNbVertices();
	const physx::PxVec3* Vertices    = ConvexMesh->getVertices();
	const physx::PxU32   PolyCount   = ConvexMesh->getNbPolygons();
	physx::PxHullPolygon Polygon;

	std::vector<MyVertexData> VertexData(VertexCount*3);
	for (physx::PxU32 i = 0; i < PolyCount; ++i)
	{
		ConvexMesh->getPolygonData(i, Polygon);

		for (physx::PxU32 j = 2; j < Polygon.mNbVerts; ++j)
		{
			physx::PxU8 i0 = ConvexMesh->getIndexBuffer()[Polygon.mIndexBase + 0];
			physx::PxU8 i1 = ConvexMesh->getIndexBuffer()[Polygon.mIndexBase + j - 1];
			physx::PxU8 i2 = ConvexMesh->getIndexBuffer()[Polygon.mIndexBase + j];

			MyVertexData V0, V1, V2;
			V0.Pos = XMFLOAT3(Vertices[i0].x,Vertices[i0].y,-Vertices[i0].z);
			V1.Pos = XMFLOAT3(Vertices[i1].x,Vertices[i1].y,-Vertices[i1].z);
			V2.Pos = XMFLOAT3(Vertices[i2].x,Vertices[i2].y,-Vertices[i2].z);

			VertexData.emplace_back(V0);
			VertexData.emplace_back(V1);
			VertexData.emplace_back(V2);
		}
	}
	VertexData.shrink_to_fit();

	// DirectX 11 버텍스 버퍼 생성
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = static_cast<UINT>(sizeof(MyVertexData) * VertexData.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = VertexData.data();

	ID3D11Buffer* pVertexBuffer = nullptr;
	GDirectXDevice->GetDevice()->CreateBuffer(&bd, &InitData, &pVertexBuffer);
	return pVertexBuffer;
}

void UPhysicsEngine::UnRegisterActor(physx::PxRigidActor* RemoveActor) const
{
	if (!RemoveActor)
	{
		return;
	}

	if (physx::PxScene* CurScene = RemoveActor->getScene())
	{
		CurScene->removeActor(*RemoveActor);
	}

	RemoveActor->release();
	RemoveActor = nullptr;
}

physx::PxConvexMesh* UPhysicsEngine::CreateConvexMesh(const std::shared_ptr<UStaticMesh>& StaticMesh) const
{
	std::vector<physx::PxVec3> Vertices;
	const std::vector<std::vector<MyVertexData>>& VertexData = StaticMesh->GetStaticMeshRenderData()->VertexData;
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
	physx::PxConvexMeshDesc ConvexMeshDesc;
	ConvexMeshDesc.points.count  = static_cast<physx::PxU32>(Vertices.size());
	ConvexMeshDesc.points.stride = sizeof(physx::PxVec3);
	ConvexMeshDesc.points.data   = Vertices.data();
	ConvexMeshDesc.flags         = physx::PxConvexFlag::eCOMPUTE_CONVEX;

	// Cooking
	physx::PxCookingParams CookingParams(PxPhysics->getTolerancesScale());
	CookingParams.planeTolerance = 0.001f;
	CookingParams.areaTestEpsilon = 0.06f;
	CookingParams.gaussMapLimit = 32;

	physx::PxDefaultMemoryOutputStream Buffer;
	bool Status = PxCookConvexMesh(CookingParams, ConvexMeshDesc, Buffer);

	if (!Status)
	{
		MY_LOG("Error", EDebugLogLevel::DLL_Error, "Can't make convex mesh from static mesh : " + StaticMesh->GetName());
		return nullptr;
		//assert(nullptr && "잘못된 결과");
	};

	physx::PxDefaultMemoryInputData input(Buffer.getData(), Buffer.getSize());
	return PxPhysics->createConvexMesh(input);
}
