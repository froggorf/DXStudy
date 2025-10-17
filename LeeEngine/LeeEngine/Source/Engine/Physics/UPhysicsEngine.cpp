#include "CoreMinimal.h"
#include "UPhysicsEngine.h"

#include "ULineComponent.h"

std::unique_ptr<UPhysicsEngine> GPhysicsEngine = nullptr;

void FPhysicsEventCallback::onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
{
}

void FPhysicsEventCallback::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
{
	for (physx::PxU32 i = 0; i < count; ++i)
	{
		const physx::PxTriggerPair& Pair = pairs[i];
		
		physx::PxActor* TriggerActor = Pair.triggerActor;
		physx::PxActor* OtherActor = Pair.otherActor;

		UShapeComponent* A = static_cast<UShapeComponent*>(TriggerActor->userData);
		UShapeComponent* B = static_cast<UShapeComponent*>(OtherActor->userData);

		
		if (Pair.status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			A->OnComponentBeginOverlap.Broadcast(A,B->GetOwner(),B);
			B->OnComponentBeginOverlap.Broadcast(B,A->GetOwner(),A);
		}
		if (Pair.status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
		{
			A->OnComponentEndOverlap.Broadcast(A,B->GetOwner(),B);
			B->OnComponentEndOverlap.Broadcast(B,A->GetOwner(),A);
		}

	}
}

void FPhysicsEventCallback::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
{
	UShapeComponent* A = static_cast<UShapeComponent*>(pairHeader.actors[0]->userData);
	UShapeComponent* B = static_cast<UShapeComponent*>(pairHeader.actors[1]->userData);

	for (physx::PxU32 i = 0; i < nbPairs; ++i)
	{
		
		const physx::PxContactPair& ContactPair = pairs[i];
		if (ContactPair.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			FHitResult HitResult;
			HitResult.HitActor = B->GetOwner();
			HitResult.HitComponent = B;

			physx::PxContactPairPoint ContactPoints[16];
			physx::PxU32 contactCount = ContactPair.extractContacts(ContactPoints, 16);

			if (contactCount > 0)
			{
				HitResult.Location = XMFLOAT3(ContactPoints[0].position.x, ContactPoints[0].position.y, -ContactPoints[0].position.z);
				HitResult.Normal   = XMFLOAT3(ContactPoints[0].normal.x,   ContactPoints[0].normal.y,   -ContactPoints[0].normal.z);
			}

			A->OnComponentHit.Broadcast(A, B->GetOwner(), B, HitResult);

			HitResult.HitActor = A->GetOwner();
			HitResult.HitComponent = A;

			B->OnComponentHit.Broadcast(B, A->GetOwner(), A, HitResult);
		}
	}
}

physx::PxFilterFlags MyFilterShader(physx::PxFilterObjectAttributes Attributes0, physx::PxFilterData FilterData0, physx::PxFilterObjectAttributes        Attributes1, physx::PxFilterData        FilterData1, physx::PxPairFlags&                    PairFlags, const void* /*constantBlock*/, physx::PxU32 /*constantBlockSize*/)
{
	physx::PxU32 Channel0 = FilterData0.word0;
	physx::PxU32 Channel1 = FilterData1.word0;

	// Block 체크
	bool Block0 = (FilterData0.word1 & Channel1) != 0;
	bool Block1 = (FilterData1.word1 & Channel0) != 0;

	// Overlap 체크
	bool Overlap0 = (FilterData0.word2 & Channel1) != 0;
	bool Overlap1 = (FilterData1.word2 & Channel0) != 0;

	if (Block0 || Block1)
	{
		// Hit 처리
		PairFlags = physx::PxPairFlag::eCONTACT_DEFAULT | physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | physx::PxPairFlag::eNOTIFY_TOUCH_LOST;
		return physx::PxFilterFlag::eDEFAULT;
	}
	else if (Overlap0 || Overlap1)
	{
		// Overlap 처리
		PairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
		return physx::PxFilterFlag::eDEFAULT;
	}
	else
	{
		// Ignore
		return physx::PxFilterFlag::eSUPPRESS;
	}
}

UPhysicsEngine::UPhysicsEngine()
{
}

UPhysicsEngine::~UPhysicsEngine()
{
	if (PxFoundation) 
	{
		PxFoundation->release();
		PxFoundation = nullptr;
	}
	//if (PxPhysics)
	//{
	//	PxPhysics->release();
	//	PxPhysics = nullptr;
	//}
	//if (PxScene)
	//{
	//	//PxScene->release();
	//	//PxScene = nullptr;
	//}
	//if (Manager)
	//{
	//	Manager->release();
	//	Manager = nullptr;
	//}
	//if (DefaultMaterial)
	//{
	//	DefaultMaterial->release();
	//	DefaultMaterial=nullptr;
	//}
}

void UPhysicsEngine::InitPhysicsEngine()
{
	bIsRegistered = false;
	PxFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	CreateScene();
}

void UPhysicsEngine::CreateScene()
{
	bIsRegistered = false;
	if (PxPhysics)
	{
		PxPhysics->release();
		PxPhysics = nullptr;
	}
	if (PxScene)
	{
		//PxScene->release();
		//PxScene = nullptr;
	}
	if (Manager)
	{
		//Manager->release();
		//Manager = nullptr;
	}
	if (DefaultMaterial)
	{
		//DefaultMaterial->release();
		//DefaultMaterial=nullptr;
	}

	PxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *PxFoundation, physx::PxTolerancesScale());

	// Scene 생성
	physx::PxSceneDesc SceneDesc(PxPhysics->getTolerancesScale());
	SceneDesc.gravity = physx::PxVec3(0.0f,-9.8f*7.5f, 0.0f);
	SceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
	SceneDesc.filterShader = MyFilterShader;

	if (!CallbackInstance)
	{
		CallbackInstance = std::make_unique<FPhysicsEventCallback>();
	}
	SceneDesc.simulationEventCallback = CallbackInstance.get();
	PxScene = PxPhysics->createScene(SceneDesc);
	if (PxScene == nullptr)
	{
		int a = 0;
	}

	Manager = PxCreateControllerManager(*PxScene);
	if (Manager == nullptr)
	{
		int a = 0;
	}

	DefaultMaterial = PxPhysics->createMaterial(0.5f, 0.5f, 0.6f); // friction, restitution

	bIsRegistered = true;
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
					// 키네마틱인 애들은 위치 정보를 갱신해주지 말아야함
					if (!RigidDynamic->getRigidBodyFlags().isSet(physx::PxRigidBodyFlag::eKINEMATIC))
					{
						const physx::PxTransform& PxTransform = RigidDynamic->getGlobalPose();
						FTransform Transform{{PxTransform.p.x, PxTransform.p.y, -PxTransform.p.z},{PxTransform.q.x, PxTransform.q.y, PxTransform.q.z, PxTransform.q.w},{1, 1, 1}};
						ShapeComp->SetWorldTransform(Transform);
					}
				}
			}
		}
	}

	
}

physx::PxShape* UPhysicsEngine::CreateSphereShape(const float Radius) const
{
	return PxPhysics->createShape(physx::PxSphereGeometry(Radius), *DefaultMaterial);
}

physx::PxShape* UPhysicsEngine::CreateBoxShape(const XMFLOAT3& BoxExtent) const
{
	return PxPhysics->createShape(physx::PxBoxGeometry(BoxExtent.x, BoxExtent.y,BoxExtent.z), *DefaultMaterial);
}

physx::PxShape* UPhysicsEngine::CreateCapsuleShape(const float Radius, const float HalfHeight) const
{
	return PxPhysics->createShape(physx::PxCapsuleGeometry(Radius,HalfHeight), *DefaultMaterial);
}


physx::PxRigidActor* UPhysicsEngine::CreateActor(const FTransform& Transform, physx::PxShape* InShape, const float Mass, bool bIsDynamic) const
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
		InShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false); 
		InShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);      
		Actor = PxPhysics->createRigidStatic(ActorTransform);
	}
	
	Actor->attachShape(*InShape);

	return Actor;
}


physx::PxRigidActor* UPhysicsEngine::CreateConvexActor(const FTransform& Transform, const std::shared_ptr<UStaticMesh>& StaticMesh, const float Mass, Microsoft::WRL::ComPtr<ID3D11Buffer>& OutVertexBuffer, bool bIsDynamic) const
{
	physx::PxConvexMesh* ConvexMesh = CreateConvexMesh(StaticMesh);
	if (!ConvexMesh)
	{
		assert(nullptr && "에러 - CreateConvexMesh");
	}

#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
	OutVertexBuffer = CreateVertexBufferForConvexActor(ConvexMesh);
#endif



	// RigidActor
	physx::PxRigidActor* Actor = nullptr;
	if (bIsDynamic)
	{
		physx::PxRigidDynamic* DynamicActor = PxPhysics->createRigidDynamic(physx::PxTransform{{Transform.Translation.x,Transform.Translation.y,-Transform.Translation.z},{-Transform.Rotation.x,-Transform.Rotation.y,Transform.Rotation.z,Transform.Rotation.w}});
		physx::PxRigidBodyExt::updateMassAndInertia(*DynamicActor, Mass);
		Actor = DynamicActor;
	}
		
	else
	{
		 
		Actor = PxPhysics->createRigidStatic(physx::PxTransform{{Transform.Translation.x,Transform.Translation.y,-Transform.Translation.z},{-Transform.Rotation.x,-Transform.Rotation.y,Transform.Rotation.z,Transform.Rotation.w}});;
	}

	// shape
	float ScaleOffset = 1.f;
	physx::PxMeshScale Scale = physx::PxVec3{Transform.Scale3D.x * ScaleOffset,Transform.Scale3D.y * ScaleOffset,Transform.Scale3D.z * ScaleOffset};
	physx::PxConvexMeshGeometry convexGeom(ConvexMesh, Scale);
	physx::PxShape*      shape = PxPhysics->createShape(convexGeom, *DefaultMaterial,true);
	shape->setContactOffset(0.009f);
	shape->setRestOffset(0.0f);
	Actor->attachShape(*shape);
	shape->release();

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

	physx::PxU32 ActorNum = PxScene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC| physx::PxActorTypeFlag::eRIGID_STATIC);
	std::vector<physx::PxActor*> Actors(ActorNum);
	PxScene->getActors(physx::PxActorTypeFlag::eRIGID_STATIC|physx::PxActorTypeFlag::eRIGID_DYNAMIC, Actors.data(), ActorNum);
	if (std::ranges::find(Actors, RemoveActor) == Actors.end	())
	{
		return;
	}

	if (RemoveActor->userData)
	{
		static_cast<UShapeComponent*>(RemoveActor->userData)->bIsAddedToPxScene = false;	
	}
	
	RemoveActorFromScene(RemoveActor);

	RemoveActor->release();
	RemoveActor = nullptr;
}

void UPhysicsEngine::RemoveActorFromScene(physx::PxRigidActor* RemoveActor) const
{
	if (PxScene == nullptr)
	{
		MY_LOG("LogTemp", EDebugLogLevel::DLL_Error, ("PxScene is nullptr!"));
		return;
	}
	if (RemoveActor == nullptr)
	{
		MY_LOG("LogTemp", EDebugLogLevel::DLL_Error, ("RemoveActor is nullptr!"));
		return;
	}
	
	PxScene->removeActor(*RemoveActor);
}

void UPhysicsEngine::AddActor(physx::PxRigidActor* AddActor) const
{
	if (PxScene && AddActor && !AddActor->getScene())
	{
		physx::PxU32 ActorNum = PxScene->getNbActors(physx::PxActorTypeFlag::eRIGID_DYNAMIC| physx::PxActorTypeFlag::eRIGID_STATIC);
		std::vector<physx::PxActor*> Actors(ActorNum);
		PxScene->getActors(physx::PxActorTypeFlag::eRIGID_STATIC|physx::PxActorTypeFlag::eRIGID_DYNAMIC, Actors.data(), ActorNum);
		if (std::ranges::find(Actors, AddActor) == Actors.end())
		{
			PxScene->addActor(*AddActor);
		}

		if (AddActor->userData)
		{
			static_cast<UShapeComponent*>(AddActor->userData)->bIsAddedToPxScene = true;	
		}
	}
}


void UPhysicsEngine::ResetScene()
{
	CreateScene();
}

physx::PxControllerManager* UPhysicsEngine::GetControllerManager()
{
	if (!Manager)
	{
		Manager = PxCreateControllerManager(*PxScene);
	}
	return Manager;
}

bool UPhysicsEngine::LineTraceSingleByChannel(const XMFLOAT3& Start, const XMFLOAT3& End, const std::vector<ECollisionChannel>& TraceChannel,FHitResult& HitResult, float DebugDrawTime, const XMFLOAT3& TraceColor, const XMFLOAT3& TraceHitColor) const
{
	if (!PxScene)
	{
		return false;
	}

	physx::PxVec3 StartVec = {Start.x,Start.y,-Start.z};
	physx::PxVec3 EndVec = {End.x,End.y,-End.z};
	physx::PxVec3 Dir = EndVec - StartVec;
	// Dir.normalize() -> Dir을 정규화하고 길이를 반환
	float Dist = Dir.normalize();
	physx::PxRaycastBuffer HitBuffer;

	physx::PxQueryFilterData FilterData;
	for (size_t i = 0; i < TraceChannel.size(); ++i)
	{
		FilterData.data.word0 |= (1 << static_cast<UINT>(TraceChannel[i]));
	}

	bool bIsHit = PxScene->raycast(StartVec,Dir,Dist, HitBuffer, physx::PxHitFlag::eDEFAULT, FilterData);
	if (bIsHit)
	{
		const physx::PxRaycastHit& HitInfo = HitBuffer.block;
		HitResult.HitComponent = static_cast<UShapeComponent*>(HitInfo.actor->userData);
		HitResult.HitActor = HitResult.HitComponent->GetOwner();
		HitResult.Location = {HitInfo.position.x,HitInfo.position.y,-HitInfo.position.z};
		HitResult.Normal = {HitInfo.normal.x,HitInfo.normal.y,-HitInfo.normal.z};
	}

#if defined(MYENGINE_BUILD_DEBUG) || defined(MYENGINE_BUILD_DEVELOPMENT)
	if (DebugDrawTime > 0.0f)
	{
		std::shared_ptr<ULineComponent> LineComp = std::make_shared<ULineComponent>(bIsHit,Start,End,HitResult.Location, TraceColor,TraceHitColor);	
		FDebugRenderData Data;
		Data.Transform = LineComp->GetComponentTransform();
		Data.RemainTime = DebugDrawTime;
		Data.ShapeComp = LineComp;
		FScene::DrawDebugData_GameThread(Data);
	}
#endif

	return bIsHit;
	
}

void UPhysicsEngine::SphereOverlapComponents(const XMFLOAT3& SpherePos, float SphereRadius, const std::vector<ECollisionChannel>& ObjectTypes, const std::vector<AActor*>& ActorsToIgnore, std::vector<AActor*>& OutActors)
{
	physx::PxSphereGeometry SphereGeom(SphereRadius);
	physx::PxTransform SphereTransform({SpherePos.x, SpherePos.y, -SpherePos.z});
	physx::PxQueryFilterData FilterData;
	FilterData.flags = physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eNO_BLOCK;
	for (size_t i = 0; i < ObjectTypes.size(); ++i)
	{
		FilterData.data.word0 |= (1 << static_cast<UINT>(ObjectTypes[i]));
	}

	std::unique_ptr<physx::PxOverlapHit[]> hitOv   = std::make_unique<physx::PxOverlapHit[]>(4096);
	int howMany = physx::PxSceneQueryExt::overlapMultiple(
		*PxScene, SphereGeom, SphereTransform, hitOv.get(), 4096,
		FilterData
	);
	if (howMany > 0)
	{
		for (int i = 0; i < howMany; ++i)
		{
			const physx::PxOverlapHit& Hit = hitOv[i];
			UShapeComponent* ShapeComp = static_cast<UShapeComponent*>(Hit.actor->userData);
			if (!ShapeComp)
			{
				continue;
			}

			AActor* Actor = ShapeComp->GetOwner();

			// IgnoreActor 체크
			if (std::find(ActorsToIgnore.begin(), ActorsToIgnore.end(), Actor) != ActorsToIgnore.end())
			{
				continue;
			}

			OutActors.emplace_back(Actor);
		}
	}
}

void UPhysicsEngine::BoxOverlapComponents(const XMFLOAT3& BoxPos, const XMFLOAT3& BoxExtent, const std::vector<ECollisionChannel>& ObjectTypes, const std::vector<AActor*>& ActorsToIgnore, std::vector<AActor*>& OutActors)
{
	physx::PxBoxGeometry BoxGeom(physx::PxVec3{BoxExtent.x,BoxExtent.y,BoxExtent.z});
	physx::PxTransform BoxTransform({BoxPos.x, BoxPos.y, -BoxPos.z});
	physx::PxQueryFilterData FilterData;
	FilterData.flags = physx::PxQueryFlag::eSTATIC | physx::PxQueryFlag::eDYNAMIC | physx::PxQueryFlag::eNO_BLOCK;
	for (size_t i = 0; i < ObjectTypes.size(); ++i)
	{
		FilterData.data.word0 |= (1 << static_cast<UINT>(ObjectTypes[i]));
	}

	std::unique_ptr<physx::PxOverlapHit[]> hitOv   = std::make_unique<physx::PxOverlapHit[]>(4096);
	int howMany = physx::PxSceneQueryExt::overlapMultiple(
		*PxScene, BoxGeom, BoxTransform, hitOv.get(), 4096,
		FilterData
	);
	if (howMany > 0)
	{
		for (int i = 0; i < howMany; ++i)
		{
			const physx::PxOverlapHit& Hit = hitOv[i];
			UShapeComponent* ShapeComp = static_cast<UShapeComponent*>(Hit.actor->userData);
			if (!ShapeComp)
			{
				continue;
			}

			AActor* Actor = ShapeComp->GetOwner();

			// IgnoreActor 체크
			if (std::find(ActorsToIgnore.begin(), ActorsToIgnore.end(), Actor) != ActorsToIgnore.end())
			{
				continue;
			}

			OutActors.emplace_back(Actor);
		}
	}
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
			Vertices.emplace_back(physx::PxVec3{Vertex.Pos.x,Vertex.Pos.y,-Vertex.Pos.z});
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
