#include <CoreMinimal.h>

#include "ATestCube.h"

#include "imgui_internal.h"
#include "Engine/UEditorEngine.h"
#include "Engine/Components/UNiagaraComponent.h"
#include "Engine/Components/USceneComponent.h"
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/Mesh/UStaticMesh.h"
#include "Engine/Physics/UPhysicsEngine.h"

rcPolyMesh* ATestCube::MyPolyMesh;
rcPolyMeshDetail* ATestCube::MyPolyDetail;
dtNavMesh* ATestCube::MyDtNavMesh;
dtNavMeshQuery* ATestCube::MyDtNavQuery;


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
	//TestCube2->SetRelativeScale3D(XMFLOAT3(100.0f,100.0f,100.0f));
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
		SMVec[i]->SetRelativeScale3D({300,20,300});
		if (i < 10)
		{
			SMVec[i]->SetRelativeLocation(XMFLOAT3{500+150*static_cast<float>(i),   19*static_cast<float>(i)-50, 200});
		}
		else
		{
			SMVec[i]->SetRelativeLocation(XMFLOAT3{500+150*static_cast<float>(i),  150  - 19*static_cast<float>(i-10), 200});
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
	SM_Ramp->SetRelativeScale3D({3,3,9});
	SM_Ramp->SetRelativeLocation({1000,-50,0});
	SM_Ramp->SetRelativeRotation(XMFLOAT3{0,90,0});
	SM_Ramp->SetCollisionObjectType(ECollisionChannel::WorldStatic);

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
		SMC->GetBodyInstance()->SetObjectType(ECollisionChannel::WorldStatic);
	}

	TestCube2->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::Physics);
	SM_Ramp->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::Physics);
	DoRecast();
}

void ATestCube::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);


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


	static bool btrue = true;
	if(ImGui::IsKeyPressed(ImGuiKey_3))
	{
		if (btrue)
		{
			btrue = false;
			ShowDebug();
		}
	}

	if (ImGui::IsKeyPressed(ImGuiKey_4))
	{
		btrue = true;
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

void ATestCube::ShowDebug()
{
	FDebugRenderData Data;
	Data.Transform = RecastConvexMesh->GetComponentTransform();
	Data.RemainTime = 10.0f;
	Data.ShapeComp = RecastConvexMesh;
	Data.DebugColor = XMFLOAT4{0.0f,0.0f,1.0f,0.3f};
	FScene::DrawDebugData_GameThread(Data);
}

bool CreateRecastPolyMesh(const std::vector<float> verts, const std::vector<int> tris, int nverts, int ntris, rcPolyMesh* LastPolyMesh, rcPolyMeshDetail* LastPolyDetail, rcPolyMesh** OutPolyMesh, rcPolyMeshDetail** OutPolyDetail)
{
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

	rcConfig m_cfg;	
	memset(&m_cfg, 0, sizeof(m_cfg));
	float Gap = 100.0f;
	m_cfg.cs = 0.3f*Gap;
	m_cfg.ch = 0.2f*Gap;
	m_cfg.walkableSlopeAngle = 45.0f;
	m_cfg.walkableHeight = (int)ceilf(2.0*Gap / m_cfg.ch);
	m_cfg.walkableClimb = (int)floorf(0.3*Gap / m_cfg.ch);
	m_cfg.walkableRadius = (int)ceilf(0.3*Gap / m_cfg.cs);
	m_cfg.maxEdgeLen = (int)(12 / m_cfg.cs);
	m_cfg.maxSimplificationError = 1.3;
	m_cfg.minRegionArea = (int)rcSqr(8);		// Note: area = size*size
	m_cfg.mergeRegionArea = (int)rcSqr(20);	// Note: area = size*size
	m_cfg.maxVertsPerPoly = (int)6;
	m_cfg.detailSampleDist = m_cfg.cs * 6;
	m_cfg.detailSampleMaxError = m_cfg.ch * 1;

	m_cfg.bmin[0] = bmin[0]; m_cfg.bmin[1] = bmin[1]; m_cfg.bmin[2] = bmin[2];
	m_cfg.bmax[0] = bmax[0]; m_cfg.bmax[1] = bmax[1]; m_cfg.bmax[2] = bmax[2];
	rcCalcGridSize(m_cfg.bmin, m_cfg.bmax, m_cfg.cs, &m_cfg.width, &m_cfg.height);
	// ========================================
	// 3. rcContext 생성
	// ========================================
	rcContext* ctx = new rcContext();

	//
	// Step 2. Rasterize input polygon soup.
	//
	// Allocate voxel heightfield where we rasterize our input data to.
	rcHeightfield* solid = rcAllocHeightfield();



	if (!solid)
	{
		assert(nullptr&&"!solid");
	}
	if (!rcCreateHeightfield(ctx, *solid, m_cfg.width, m_cfg.height, m_cfg.bmin, m_cfg.bmax, m_cfg.cs, m_cfg.ch))
	{
		assert(nullptr&&"!rcCreateHeightfield");
		return false;
	}

	// Allocate array that can hold triangle area types.
	// If you have multiple meshes you need to process, allocate
	// and array which can hold the max number of triangles you need to process.
	unsigned char* triAreas = new unsigned char[ntris]; // 삼각형별 걷기 가능/불가능 정보

	// Find triangles which are walkable based on their slope and rasterize them.
	// If your input data is multiple meshes, you can transform them here, calculate
	// the are type for each of the meshes and rasterize them.
	memset(triAreas, 0, ntris*sizeof(unsigned char));
	rcMarkWalkableTriangles(ctx, m_cfg.walkableSlopeAngle, verts.data(), nverts, tris.data(), ntris, triAreas);
	if (!rcRasterizeTriangles(ctx, verts.data(), nverts, tris.data(), triAreas, ntris, *solid, m_cfg.walkableClimb))
	{
		assert(nullptr && "!rcRasterizeTriangles");
		return false;
	}

	//
	// Step 3. Filter walkable surfaces.
	//

	// Once all geometry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	rcFilterLowHangingWalkableObstacles(ctx, m_cfg.walkableClimb, *solid);
	rcFilterLedgeSpans(ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *solid);
	rcFilterWalkableLowHeightSpans(ctx, m_cfg.walkableHeight, *solid);


	//
	// Step 4. Partition walkable surface to simple regions.
	//

	// Compact the heightfield so that it is faster to handle from now on.
	// This will result more cache coherent data as well as the neighbours
	// between walkable cells will be calculated.
	rcCompactHeightfield* chf = rcAllocCompactHeightfield();
	if (!rcBuildCompactHeightfield(ctx, m_cfg.walkableHeight, m_cfg.walkableClimb, *solid, *chf))
	{
		assert(nullptr && "!rcBuildCompactHeightField");
		return false;
	}

	// Erode the walkable area by agent radius.
	if (!rcErodeWalkableArea(ctx, m_cfg.walkableRadius, *chf))
	{
		assert(nullptr && "!rcErodeWalkableArea");
		return false;
	}

	// ========================================
	// 8. 거리 필드 & 지역 분할
	// ========================================
	if (!rcBuildDistanceField(ctx, *chf)) {
		assert(nullptr && "!rcBuildDistanceField");
		return false;
	}
	if (!rcBuildRegions(ctx, *chf, 0, m_cfg.minRegionArea, m_cfg.mergeRegionArea)) {
		assert(nullptr && "!rcBuildRegions");
		return false;
	}

	// ========================================
	// 9. 윤곽선(Contour) 생성
	// ========================================
	rcContourSet* cset = rcAllocContourSet();
	if (!rcBuildContours(ctx, *chf, m_cfg.maxSimplificationError, m_cfg.maxEdgeLen, *cset))
	{
		assert(nullptr && "!rcBuildContours");
		return false;
	}

	// ========================================
	// 10. 폴리곤 메시 생성
	// ========================================
	rcPolyMesh* pmesh = rcAllocPolyMesh();
	if (!rcBuildPolyMesh(ctx, *cset, m_cfg.maxVertsPerPoly, *pmesh))
	{
		assert(nullptr && "!rcBuildPolyMesh");
		return false;
	}

	// ========================================
	// 11. 디테일 메시 생성
	// ========================================
	rcPolyMeshDetail* dmesh = rcAllocPolyMeshDetail();
	if (!rcBuildPolyMeshDetail(ctx, *pmesh, *chf, m_cfg.detailSampleDist, m_cfg.detailSampleMaxError, *dmesh))
	{
		assert(nullptr && "!rcBuildPolyMeshDetail");
		return false;
	}


	if(LastPolyMesh)
	{
		std::vector<rcPolyMesh*> PolyMeshes { LastPolyMesh, pmesh };
		rcPolyMesh* mergedMesh = rcAllocPolyMesh();
		if (!rcMergePolyMeshes(ctx, PolyMeshes.data(), PolyMeshes.size(), *mergedMesh))
		{
			assert(nullptr && "!rcMergePolyMeshes");
			return false;
		}
		rcFreePolyMesh(pmesh); // 이전 pmesh는 더 이상 필요 없음
		pmesh = mergedMesh;

		rcFreePolyMeshDetail(dmesh);
		dmesh = rcAllocPolyMeshDetail();
		if(!rcBuildPolyMeshDetail(ctx, *pmesh, *chf, m_cfg.detailSampleDist, m_cfg.detailSampleMaxError, *dmesh))
		{
			assert(nullptr && "!rcBuildPolyMeshDetail");
			return false;
		}
	}

	*OutPolyMesh = pmesh;
	*OutPolyDetail =dmesh;
	return true;
}

void ATestCube::DoRecast()
{
	rcPolyMesh* CurPolyMesh = nullptr;
	rcPolyMeshDetail* CurPolyDetail = nullptr;

	std::vector<float> verts;
	std::vector<int> tris;

	int vertexOffset = 0;

	// 오브젝트 리스트 반복 (TestCube2, SM_Ramp 등)
	for (const std::shared_ptr<UStaticMeshComponent>& Obj : {TestCube2, SM_Ramp, 	TestCubeSM1	,
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
		TestCubeSM21,})
	{
		if (std::shared_ptr<UConvexComponent> ConvexComp = std::dynamic_pointer_cast<UConvexComponent>(Obj->GetBodyInstance()))
		{
			FStaticMeshRenderData* Data = ConvexComp->GetStaticMesh()->GetStaticMeshRenderData();
			const std::vector<std::vector<MyVertexData>>& VertexData = Data->VertexData;
			const XMMATRIX& TransformMatrix = Obj->GetComponentTransform().ToMatrixWithScale();

			// === 1. 정점 push ===
			int thisMeshVertCount = 0;
			for (const auto& VertexPerMesh : VertexData)
			{
				for (const MyVertexData& Vertex : VertexPerMesh)
				{
					XMVECTOR VertexVec = XMLoadFloat3(&Vertex.Pos);
					VertexVec = XMVector3TransformCoord(VertexVec, TransformMatrix);
					verts.push_back(XMVectorGetX(VertexVec));
					verts.push_back(XMVectorGetY(VertexVec));
					verts.push_back(-XMVectorGetZ(VertexVec)); // 좌표계 변환 필요시
					thisMeshVertCount++;
				}
			}

			// === 2. 인덱스 push ===
			const std::vector<std::vector<uint32_t>>& Indices = Data->IndexData;
			int subVertOffset = 0;
			for (size_t meshIdx = 0; meshIdx < VertexData.size(); ++meshIdx)
			{
				const auto& SubIndices = Indices[meshIdx];
				for (size_t i = 0; i < SubIndices.size(); i += 3)
				{
					tris.push_back(vertexOffset + subVertOffset + SubIndices[i + 0]);
					tris.push_back(vertexOffset + subVertOffset + SubIndices[i + 1]);
					tris.push_back(vertexOffset + subVertOffset + SubIndices[i + 2]);
				}
				subVertOffset += VertexData[meshIdx].size();
			}

			// === 3. vertexOffset 누적 ===
			vertexOffset += thisMeshVertCount;
		}
	}


	int nverts = verts.size() / 3;
	int ntris = tris.size() / 3;


	CreateRecastPolyMesh(verts, tris,  verts.size()/3, tris.size()/3, nullptr, nullptr, &CurPolyMesh, &CurPolyDetail);

	
	RecastConvexMesh = std::make_shared<UConvexComponent>();
	RecastConvexMesh->CreateVertexBufferForNavMesh(CurPolyDetail);

	ShowDebug();

	// ========================================
	// 12. Detour 데이터로 변환 (선택)
	// ========================================
	// Detour를 사용한다면 pmesh, dmesh 내용을 Detour NavMesh로 변환

	MyPolyMesh = CurPolyMesh;
	MyPolyDetail = CurPolyDetail;

	CreateDetour();

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

void ATestCube::CreateDetour(){

	dtNavMeshCreateParams params = {};
	memset(&params, 0, sizeof(params));

	// rcPolyMesh에서 정보 복사
	params.verts = MyPolyMesh->verts;
	params.vertCount = MyPolyMesh->nverts;
	params.polys = MyPolyMesh->polys;
	params.polyAreas = MyPolyMesh->areas;
	unsigned short* polyFlags = new unsigned short[params.polyCount];
	for (int i = 0; i < params.polyCount; ++i)
	{
		// 모든 폴리곤을 "걷기 가능"으로 세팅
		polyFlags[i] = 0x01;
	}
	params.polyFlags = polyFlags;
	params.polyCount = MyPolyMesh->npolys;
	params.nvp = MyPolyMesh->nvp;

	// rcPolyMeshDetail에서 정보 복사
	params.detailMeshes = MyPolyDetail->meshes;
	params.detailVerts = MyPolyDetail->verts;
	params.detailVertsCount = MyPolyDetail->nverts;
	params.detailTris = MyPolyDetail->tris;
	params.detailTriCount = MyPolyDetail->ntris;

	float Gap = MyPolyMesh->ch/0.2f;
	// 기타 옵션
	params.walkableHeight = (int)ceilf(2.0*Gap / MyPolyMesh->ch);
	params.walkableClimb = (int)ceilf(0.3*Gap / MyPolyMesh->ch);
	params.walkableRadius = (int)floorf(0.3*Gap / MyPolyMesh->cs);
	params.tileX = 0;
	params.tileY = 0;
	params.tileLayer = 0;
	params.bmin[0] = MyPolyMesh->bmin[0];
	params.bmin[1] = MyPolyMesh->bmin[1];
	params.bmin[2] = MyPolyMesh->bmin[2];
	params.bmax[0] = MyPolyMesh->bmax[0];
	params.bmax[1] = MyPolyMesh->bmax[1];
	params.bmax[2] = MyPolyMesh->bmax[2];
	params.cs = MyPolyMesh->cs;
	params.ch = MyPolyMesh->ch;
	params.buildBvTree = false; // BV트리 사용 여부
	

	unsigned char* navData = nullptr;
	int navDataSize = 0;

	if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
	{
		assert(nullptr&& "!dtCreateNavMeshData");
		return;
	}

	dtNavMesh* navMesh = dtAllocNavMesh();
	if (!navMesh)
	{
		assert(nullptr&& "!dtCreateNavMeshData");
		return;
	}

	dtStatus status = navMesh->init(navData, navDataSize, DT_TILE_FREE_DATA);
	if (dtStatusFailed(status))
	{
		dtFree(navMesh);
		assert(nullptr&& "!dtCreateNavMeshData");
		return;
	}

	MyDtNavMesh = navMesh;
	MyDtNavQuery = dtAllocNavMeshQuery();
	MyDtNavQuery->init(MyDtNavMesh, 4096*4);
}