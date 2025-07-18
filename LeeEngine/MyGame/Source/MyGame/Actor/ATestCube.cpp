#include <CoreMinimal.h>

#include "ATestCube.h"

#include "imgui_internal.h"
#include "Engine/UEditorEngine.h"
#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/Components/USceneComponent.h"
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/Mesh/UStaticMesh.h"
#include "Engine/Physics/UPhysicsEngine.h"

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
	TestCube2->SetRelativeScale3D(XMFLOAT3(1600.0f, 0.01f, 1600.0f));
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
	for (size_t i = 0; i < SMVec.size(); ++i)
	{
		SMVec[i]->SetupAttachment(GetRootComponent());
		SMVec[i]->SetRelativeScale3D({100,20,100});
		if (i < 10)
		{
			SMVec[i]->SetRelativeLocation(XMFLOAT3{500+80*static_cast<float>(i),   19*static_cast<float>(i), 200});
		}
		else
		{
			SMVec[i]->SetRelativeLocation(XMFLOAT3{500+80*static_cast<float>(i),  200  - 19*static_cast<float>(i-10), 200});
		}
		AssetManager::GetAsyncAssetCache("SM_Box", [SMVec,i](std::shared_ptr<UObject> Object)
		{
				SMVec[i]->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));	
		});
		
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

	SM_Ramp = std::make_shared<UStaticMeshComponent>();
	AssetManager::GetAsyncAssetCache("SM_Ramp", [this](std::shared_ptr<UObject> Object)
	{
		SM_Ramp->SetStaticMesh(std::static_pointer_cast<UStaticMesh>(Object));
	});
	SM_Ramp->SetupAttachment(GetRootComponent());
	SM_Ramp->SetRelativeScale3D({1,1,3});
	SM_Ramp->SetRelativeLocation({500,-50,0});
	SM_Ramp->SetRelativeRotation(XMFLOAT3{0,90,0});
	

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
	for (const std::shared_ptr<UStaticMeshComponent>& SMC : SMVec)
	{
		SMC->GetBodyInstance()->SetSimulatePhysics(false);
		SMC->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::Physics);
	}

	TestCube2->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::Physics);

	DoRecast();
}

void ATestCube::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);



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
	//DummyComp->SetWorldLocation(Loc);


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

void ATestCube::DoRecast()
{
	// ========================================
	// 1. 입력 데이터 준비
	// ========================================

	std::vector<float> verts;
	std::vector<int> tris;
	
	if (std::shared_ptr<UConvexComponent> ConvexComp = std::dynamic_pointer_cast<UConvexComponent>(TestCube2->GetBodyInstance()))
	{
		FStaticMeshRenderData* Data = ConvexComp->GetStaticMesh()->GetStaticMeshRenderData();
		const std::vector<std::vector<MyVertexData>>& VertexData = Data->VertexData;
		const XMMATRIX& TransformMatrix = TestCube2->GetComponentTransform().ToMatrixWithScale();
		for (const auto& VertexPerMesh : VertexData)
		{
			for (const MyVertexData& Vertex : VertexPerMesh)
			{
				XMVECTOR VertexVec = XMLoadFloat3(&Vertex.Pos);
				VertexVec = XMVector3TransformCoord(VertexVec, TransformMatrix);
				verts.push_back(XMVectorGetX(VertexVec));
				verts.push_back(XMVectorGetY(VertexVec));
				verts.push_back(-XMVectorGetZ(VertexVec)); // 좌표계 변환 필요시
			}
		}

		const std::vector<std::vector<uint32_t>>& Indices = Data->IndexData;
		int vertexOffset = 0;
		for (const auto& SubIndices : Indices)
		{
			for (size_t i = 0; i < SubIndices.size(); i += 3)
			{
				tris.push_back(vertexOffset + SubIndices[i + 0]);
				tris.push_back(vertexOffset + SubIndices[i + 1]);
				tris.push_back(vertexOffset + SubIndices[i + 2]);
			}
			vertexOffset += VertexData[&SubIndices - &Indices[0]].size(); // 각 submesh별 offset
		}
	}
	int nverts = verts.size() / 3;
	int ntris = tris.size() / 3;
	

	// AABB 구하기
	float bmin[3] = { verts[0], verts[1], verts[2] };
	float bmax[3] = { verts[0], verts[1], verts[2] };
	for (int i = 0; i < nverts; ++i)
	{
		float x = verts[i * 3 + 0];
		float y = verts[i * 3 + 1];
		float z = verts[i * 3 + 2];
		if (x < bmin[0]) bmin[0] = x;
		if (y < bmin[1]) bmin[1] = y;
		if (z < bmin[2]) bmin[2] = z;
		if (x > bmax[0]) bmax[0] = x;
		if (y > bmax[1]) bmax[1] = y;
		if (z > bmax[2]) bmax[2] = z;
	}
	// ========================================
	// 2. 주요 파라미터 설정 (추천값 포함)
	// ========================================

	float cellSize = 3.f;           // 셀 가로/세로 길이 (월드 단위, 0.2~0.3 권장)
	float cellHeight = 2.f;         // 셀 높이 (월드 단위, 0.1~0.2 권장)
	float walkableSlopeAngle = 45.0f; // 걷기 허용 최대 경사각 (도 단위, 30~50 권장)
	float walkableHeight = 2.0f;     // 캐릭터가 설 수 있는 최소 천장 높이 (미터, 1.8~2.0 권장)
	float walkableClimb = 0.4f;      // 넘을 수 있는 최대 턱 높이 (미터, 0.3~0.4 권장)
	int borderSize = 8;              // 내비메쉬 경계 여유 셀 수 (8 권장)
	int minRegionArea = rcSqr(8);    // 최소 지역 크기 (셀 수, 8x8 권장)
	int mergeRegionArea = rcSqr(20); // 병합 지역 크기 (셀 수, 20x20 권장)
	float maxSimplificationError = 1.3f; // 윤곽선 단순화 오차 (1.3 권장)
	int maxEdgeLen = 12;             // 윤곽선 최대 에지 길이 (12 권장)
	int nvp = 6;                     // 폴리곤당 최대 꼭짓점 수 (6 권장)
	float sampleDist = 6.0f;         // 디테일 메시 샘플 간격 (6.0 권장)
	float sampleMaxError = 1.0f;     // 디테일 메시 최대 오차 (1.0 권장)

	// ========================================
	// 3. rcContext 생성
	// ========================================
	rcContext* ctx = new rcContext();

	// ========================================
	// 4. rcHeightfield 생성
	// ========================================
	int width = (int)((bmax[0] - bmin[0]) / cellSize + 0.5f);  // X축 셀 개수
	int height = (int)((bmax[2] - bmin[2]) / cellSize + 0.5f); // Z축 셀 개수

	rcHeightfield* solid = rcAllocHeightfield();
	if (!rcCreateHeightfield(ctx, *solid, width, height, bmin, bmax, cellSize, cellHeight)) {
		// 실패 처리
	}

	// ========================================
	// 5. 걷기 가능 삼각형 마킹 및 rasterize
	// ========================================
	unsigned char* triAreas = new unsigned char[ntris]; // 삼각형별 걷기 가능/불가능 정보

	// 걷기 가능한 삼각형 마킹
	rcMarkWalkableTriangles(ctx, walkableSlopeAngle, verts.data(), nverts, tris.data(), ntris, triAreas);

	// 삼각형을 높이필드에 rasterize
	rcRasterizeTriangles(ctx, verts.data(), nverts, tris.data(), triAreas, ntris, *solid, walkableClimb);

	// ========================================
	// 6. 필터링 (낮은 천장, 낭떠러지, 장애물 등)
	// ========================================
	rcFilterLowHangingWalkableObstacles(ctx, walkableClimb, *solid);
	rcFilterLedgeSpans(ctx, walkableHeight, walkableClimb, *solid);
	rcFilterWalkableLowHeightSpans(ctx, walkableHeight, *solid);

	// ========================================
	// 7. Compact Heightfield 생성
	// ========================================
	rcCompactHeightfield* chf = rcAllocCompactHeightfield();
	if (!rcBuildCompactHeightfield(ctx, walkableHeight, walkableClimb, *solid, *chf)) {
		// 실패 처리
	}

	// ========================================
	// 8. 거리 필드 & 지역 분할
	// ========================================
	if (!rcBuildDistanceField(ctx, *chf)) {
		// 실패 처리
	}
	if (!rcBuildRegions(ctx, *chf, borderSize, minRegionArea, mergeRegionArea)) {
		// 실패 처리
	}

	// ========================================
	// 9. 윤곽선(Contour) 생성
	// ========================================
	rcContourSet* cset = rcAllocContourSet();
	if (!rcBuildContours(ctx, *chf, maxSimplificationError, maxEdgeLen, *cset)) {
		// 실패 처리
	}

	// ========================================
	// 10. 폴리곤 메시 생성
	// ========================================
	rcPolyMesh* pmesh = rcAllocPolyMesh();
	if (!rcBuildPolyMesh(ctx, *cset, nvp, *pmesh)) {
		// 실패 처리
	}

	// ========================================
	// 11. 디테일 메시 생성
	// ========================================
	rcPolyMeshDetail* dmesh = rcAllocPolyMeshDetail();
	if (!rcBuildPolyMeshDetail(ctx, *pmesh, *chf, sampleDist, sampleMaxError, *dmesh)) {
		// 실패 처리
	}

	std::shared_ptr<UConvexComponent> ConvexComp= std::make_shared<UConvexComponent>();
	FDebugRenderData Data;
	Data.Transform = ConvexComp->GetComponentTransform();
	ConvexComp->CreateVertexBufferForNavMesh(dmesh);
	Data.RemainTime = 100.0f;
	Data.ShapeComp = ConvexComp;
	Data.DebugColor = XMFLOAT4{0.0f,0.0f,1.0f,0.3f};
	FScene::DrawDebugData_GameThread(Data);

	// ========================================
	// 12. Detour 데이터로 변환 (선택)
	// ========================================
	// Detour를 사용한다면 pmesh, dmesh 내용을 Detour NavMesh로 변환



	// ========================================
	// 13. 메모리 해제
	// ========================================
	/*delete[] triAreas;
	rcFreeHeightField(solid);
	rcFreeCompactHeightfield(chf);
	rcFreeContourSet(cset);
	rcFreePolyMesh(pmesh);
	rcFreePolyMeshDetail(dmesh);
	delete ctx;*/
}
