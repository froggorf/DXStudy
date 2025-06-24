#include "CoreMinimal.h"
#include "UPhysicsEngine.h"

std::unique_ptr<UPhysicsEngine> gPhysicsEngine = nullptr;

UPhysicsEngine::UPhysicsEngine()
{
	PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	PxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *PxFoundation, physx::PxTolerancesScale());
	

	physx::PxSceneDesc SceneDesc(PxPhysics->getTolerancesScale());
	SceneDesc.gravity = physx::PxVec3(0.0f,-9.8f*7.5, 0.0f);
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
	static float UpdateTime = 1.0 / UpdatePerSecond;
	if (CurFrameTime >= UpdateTime)
	{
		if (PxScene)
		{
			PxScene->simulate(UpdateTime);
			PxScene->fetchResults(true);
		}
		CurFrameTime -= UpdateTime;
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


physx::PxRigidActor* UPhysicsEngine::CreateAndRegisterConvexActor(const FTransform& Transform, const std::shared_ptr<UStaticMesh>& StaticMesh, const float Mass, bool bIsDynamic) const
{
	physx::PxConvexMesh* ConvexMesh = CreateConvexMesh(StaticMesh);

	// Shape
	physx::PxMeshScale Scale = physx::PxVec3{Transform.Scale3D.x,Transform.Scale3D.y,Transform.Scale3D.z};
	physx::PxConvexMeshGeometry convexGeom(ConvexMesh, Scale);
	physx::PxShape*      shape = PxPhysics->createShape(convexGeom, *DefaultMaterial);

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
