#include "CoreMinimal.h"
#include "FNiagaraEmitter.h"

using namespace Microsoft::WRL;

void FParticleModule::LoadDataFromFile(const nlohmann::basic_json<>& Data)
{
	// SpawnRate
	if (Data.contains("SpawnRate"))
	{
		SpawnRate = Data["SpawnRate"];
	}

	// Life
	if (Data.contains("Life"))
	{
		auto LifeData = Data["Life"];
		MinLife       = LifeData[0];
		MaxLife       = LifeData[1];
	}

	// bIsLoop
	if (Data.contains("Loop"))
	{
		int bIsLoop = Data["Loop"];
		this->bIsLoop     = bIsLoop;
	}

	// Scale
	if (Data.contains("Scale"))
	{
		auto ScaleData    = Data["Scale"];
		auto MinScaleData = ScaleData[0];
		auto MaxScaleData = ScaleData[1];
		MinScale          = XMFLOAT3{MinScaleData[0], MinScaleData[1], MinScaleData[2]};
		MaxScale          = XMFLOAT3{MaxScaleData[0], MaxScaleData[1], MaxScaleData[2]};
	}

	// Rotation
	if (Data.contains("Rot"))
	{
		const auto& RotData    = Data["Rot"];
		const auto& MinRotData = RotData[0];
		const auto& MaxRotData = RotData[1];
		MinRotation            = XMFLOAT3{MinRotData[0], MinRotData[1], MinRotData[2]};
		MaxRotation            = XMFLOAT3{MaxRotData[0], MaxRotData[1], MaxRotData[2]};
	}

	//SpaceType
	if (Data.contains("SpaceType"))
	{
		SpaceType = Data["SpaceType"];
	}

	// Spawn Shape
	if (Data.contains("SpawnShape"))
	{
		SpawnShape = Data["SpawnShape"];
	}

	// SpawnShapeScale
	if (Data.contains("SpawnScale"))
	{
		const auto& SpawnShapeScale = Data["SpawnScale"];
		this->SpawnShapeScale       = XMFLOAT3{SpawnShapeScale[0], SpawnShapeScale[1], SpawnShapeScale[2]};
	}

	// 모듈값 지정 시작
	if (Data.contains("Modules"))
	{
		auto ModuleData = Data["Modules"];

		//Render 모듈
		if (ModuleData.contains("Render"))
		{
			Module[static_cast<int>(EParticleModule::PM_Render)] = 1;
			const auto& RenderData = ModuleData["Render"];
			if (RenderData.contains("RandomStart"))
			{
				bStartColorRandom = RenderData["RandomStart"];
				const auto& MinColor = RenderData["MinStartColor"];
				const auto& MaxColor = RenderData["MaxStartColor"];
				StartColorMin = {MinColor[0],MinColor[1],MinColor[2],MinColor[3]};
				StartColorMax = {MaxColor[0],MaxColor[1],MaxColor[2],MaxColor[3]};
			}
			if (RenderData.contains("StartColor"))
			{
				const auto& Colors = RenderData["StartColor"];
				StartColor  = XMFLOAT4{Colors[0], Colors[1], Colors[2], Colors[3]};
			}
			if (RenderData.contains("EndColor"))
			{
				auto Colors = RenderData["EndColor"];
				EndColor    = XMFLOAT4{Colors[0], Colors[1], Colors[2], Colors[3]};
			}
			if (RenderData.contains("FadeOut"))
			{
				const auto& FadeData = RenderData["FadeOut"];
				FadeOut              = FadeData[0];
				StartRatio           = FadeData[1];
			}
		}

		// Scale 모듈
		if (ModuleData.contains("Scale"))
		{
			Module[static_cast<int>(EParticleModule::PM_Scale)] = 1;
			auto ScaleData                                      = ModuleData["Scale"];
			StartScale                                          = ScaleData["StartScale"];
			EndScale                                            = ScaleData["EndScale"];
		}

		// UVAnim 모듈
		if (ModuleData.contains("UVAnim"))
		{
			Module[static_cast<int>(EParticleModule::PM_UVAnim)] = 1;
			const auto& UVData                                   = ModuleData["UVAnim"];
			UCount                                               = UVData["UCount"];
			VCount                                               = UVData["VCount"];
		}

		// AddVelocity 모듈
		if (ModuleData.contains("AddVelocity"))
		{
			Module[static_cast<int>(EParticleModule::PM_AddVelocity)] = 1;
			const auto& UVData                                        = ModuleData["AddVelocity"];
			const auto& MinVel                                        = UVData["MinVel"];
			const auto& MaxVel                                        = UVData["MaxVel"];
			AddMinSpeed                                               = XMFLOAT3{MinVel[0], MinVel[1], MinVel[2]};
			AddMaxSpeed                                               = XMFLOAT3{MaxVel[0], MaxVel[1], MaxVel[2]};
		}

		// Add Rotation 모듈
		if (ModuleData.contains("AddRot"))
		{
			Module[static_cast<int>(EParticleModule::PM_AddRotation)] = 1;
			const auto& AddRotData                                    = ModuleData["AddRot"];
			AddRotation                                               = XMFLOAT3{AddRotData[0], AddRotData[1], AddRotData[2]};
		}

		// AddTickVelocity 모듈
		if (ModuleData.contains("TickVel"))
		{
			Module[static_cast<int>(EParticleModule::PM_AddTickVelocity)] = 1;
			const auto& AddTickVelData                                    = ModuleData["TickVel"];
			AddTickVelocity                                               = XMFLOAT3{AddTickVelData[0], AddTickVelData[1], AddTickVelData[2]};
		}

		// DynamicParam 모듈
		if (ModuleData.contains("DynamicParam"))
		{
			Module[static_cast<int>(EParticleModule::PM_DynamicParam)] = 1;
			const auto& DynamicParamModuleData = ModuleData["DynamicParam"];
			const auto& Min = DynamicParamModuleData["Min"];
			const auto& Max = DynamicParamModuleData["Max"];
			DynamicParamMin.x = Min[0]; DynamicParamMax.x = Max[0];
			DynamicParamMin.y = Min[1]; DynamicParamMax.y = Max[1];
			DynamicParamMin.z = Min[2]; DynamicParamMax.z = Max[2];
			DynamicParamMin.w = Min[3]; DynamicParamMax.w = Max[3];
		}

		// Orbit 모듈
		if (ModuleData.contains("Orbit"))
		{
			Module[static_cast<int>(EParticleModule::PM_Orbit)] = 1;
			// Speed -> rad 기준, 2.0 -> 2.0rad -> 1초에 약 0.3바퀴 / 6.0 -> 6.0rad -> 1초에 약 1바퀴
			const auto& OrbitData = ModuleData["Orbit"];
			const auto& SpeedData = OrbitData["Speed"];
			const auto& RadiusData = OrbitData["Radius"];
			OrbitRadiusMin = RadiusData[0];
			OrbitRadiusMax = RadiusData[1];
			OrbitSpeedMin = SpeedData[0];
			OrbitSpeedMax = SpeedData[1];
		}
	}
}

void FNiagaraRendererProperty::Render()
{
	size_t TextureSize = OverrideTextures.size();
	for (int i = 0; i < TextureSize; ++i)
	{
		GDirectXDevice->GetDeviceContext()->PSSetShaderResources(i, 1, OverrideTextures[i]->GetSRV().GetAddressOf());
	}
}

void FNiagaraRendererProperty::SetParticleTextures(const nlohmann::basic_json<>& Data)
{
	OverrideTextures.clear();

	size_t TextureCount = Data.size();
	OverrideTextures.resize(TextureCount);
	for (int i = 0; i < TextureCount; ++i)
	{
		OverrideTextures[i] = UTexture::GetTextureCache(Data[i]);
	}
}

void FNiagaraRendererProperty::LoadDataFromFile(const nlohmann::basic_json<>& Data)
{
	// Override Material 세팅
	if (Data.contains("OverrideMat"))
	{
		std::string_view MaterialName = Data["OverrideMat"];
		const std::shared_ptr<UMaterialInterface>& OverrideMat = UMaterialInterface::GetMaterialCache(MaterialName.data());
		assert(OverrideMat);
		SetMaterialInterface(OverrideMat);
	}

	// Override Texture 세팅
	if (Data.contains("OverrideTex"))
	{
		// 머테리얼 인스턴스에 등록을 하는 방향으로 하려했으나,
		// 새로 머테리얼인스턴스를 생성하는 방식으로 인해 실패
		// 따라서 RenderData 내에 데이터를 넣고, 렌더링 시 바인딩 하는 방향으로 진행
		SetParticleTextures(Data["OverrideTex"]);
	}
}

FNiagaraRendererBillboardSprites::FNiagaraRendererBillboardSprites()
{
	MaterialInterface = UMaterial::GetMaterialCache("MI_NiagaraBillboardSprite");
	StaticMesh        = UStaticMesh::GetStaticMesh("SM_Point");
	if (!StaticMesh)
	{
		AssetManager::ReadMyAsset(AssetManager::GetAssetNameAndAssetPathMap()["SM_Point"]);	
		StaticMesh = UStaticMesh::GetStaticMesh("SM_Point");
	}
}

// ================= Niagara Renderer ================
void FNiagaraRendererBillboardSprites::Render()
{
	if (!StaticMesh)
	{
		return;
	}

	FNiagaraRendererProperty::Render();

	GDirectXDevice->SetDSState(EDepthStencilStateType::NO_WRITE);
	GDirectXDevice->SetBSState(EBlendStateType::BST_AlphaBlend);
	auto DeviceContext = GDirectXDevice->GetDeviceContext();

	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	
	auto RenderData = StaticMesh->GetStaticMeshRenderData();
	UINT MeshIndex  = 0;
	// 셰이더 설정
	UINT stride = sizeof(MyVertexData);
	UINT offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, RenderData->VertexBuffer[MeshIndex].GetAddressOf(), &stride, &offset);
	DeviceContext->IASetIndexBuffer(RenderData->IndexBuffer[MeshIndex].Get(), DXGI_FORMAT_R32_UINT, 0);

	D3D11_BUFFER_DESC indexBufferDesc;
	RenderData->IndexBuffer[MeshIndex]->GetDesc(&indexBufferDesc);
	UINT indexSize = indexBufferDesc.ByteWidth / sizeof(UINT);
	DeviceContext->DrawIndexedInstanced(1,MaxParticleCount, 0, 0, 0);

	GDirectXDevice->SetDSState(EDepthStencilStateType::LESS);
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void FNiagaraRendererMeshes::Render()
{
	if (!BaseStaticMesh)
	{
		return;
	}

	FNiagaraRendererProperty::Render();
	GDirectXDevice->SetDSState(EDepthStencilStateType::NO_WRITE);
	GDirectXDevice->SetBSState(EBlendStateType::BST_AlphaBlend);

	auto DeviceContext = GDirectXDevice->GetDeviceContext();
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto RenderData = BaseStaticMesh->GetStaticMeshRenderData();
	for (UINT MeshIndex = 0; MeshIndex < RenderData->MeshCount; ++MeshIndex)
	{
		// 셰이더 설정
		UINT stride = sizeof(MyVertexData);
		UINT offset = 0;
		DeviceContext->IASetVertexBuffers(0, 1, RenderData->VertexBuffer[MeshIndex].GetAddressOf(), &stride, &offset);
		DeviceContext->IASetIndexBuffer(RenderData->IndexBuffer[MeshIndex].Get(), DXGI_FORMAT_R32_UINT, 0);

		D3D11_BUFFER_DESC indexBufferDesc;
		RenderData->IndexBuffer[MeshIndex]->GetDesc(&indexBufferDesc);
		UINT indexSize = indexBufferDesc.ByteWidth / sizeof(UINT);
		DeviceContext->DrawIndexedInstanced(indexSize,MaxParticleCount, 0, 0, 0);
	}

	GDirectXDevice->SetDSState(EDepthStencilStateType::LESS);
}

void FNiagaraRendererMeshes::LoadDataFromFile(const nlohmann::basic_json<>& Data)
{
	FNiagaraRendererProperty::LoadDataFromFile(Data);
	if (Data.contains("StaticMesh"))
	{
		const std::string& StaticMeshName = Data["StaticMesh"];
		std::shared_ptr<UStaticMesh> StaticMesh = UStaticMesh::GetStaticMesh(StaticMeshName);
		if (!StaticMesh)
		{
			AssetManager::ReadMyAsset(AssetManager::GetAssetNameAndAssetPathMap()[StaticMeshName]);	
			StaticMesh = UStaticMesh::GetStaticMesh(StaticMeshName);
		}
		SetStaticMesh(StaticMesh);
	}
}

void FNiagaraRendererRibbons::Render()
{
	// FNiagaraRibbonEmitter 에서 렌더링을 진행
}

// ===========================================================================================

// ==================================== Niagara Emitter ========================================

std::shared_ptr<FTickParticleCS> FNiagaraEmitter::TickParticleCS;

FNiagaraEmitter::FNiagaraEmitter()
{
	// Particle Tick 컴퓨트 셰이더 생성
	if (nullptr == TickParticleCS)
	{
		TickParticleCS = std::make_shared<FTickParticleCS>();
	}

	// 파티클 구조화 버퍼 생성
	ParticleBuffer = std::make_shared<FStructuredBuffer>();
	ParticleBuffer->Create(sizeof(FParticleData), MaxParticleCount, SB_TYPE::SRV_UAV, false);

	SpawnBuffer = std::make_shared<FStructuredBuffer>();
	SpawnBuffer->Create(sizeof(FParticleSpawn), 1, SB_TYPE::SRV_UAV, true);

	/**/

	ModuleBuffer = std::make_shared<FStructuredBuffer>();
	ModuleBuffer->Create(sizeof(FParticleModule), 1, SB_TYPE::SRV_ONLY, true, &Module);

	AccTime = 0;
}

void FNiagaraEmitter::Tick(float DeltaSeconds, const FTransform& SceneTransform)
{
	Module.ObjectWorldPos = SceneTransform.GetTranslation();

	// 이번 프레임에 활성화 될 파티클 수 계산
	CalcSpawnCount(DeltaSeconds);

	// ComputeShader 바인딩
	ModuleBuffer->SetData(&Module);
	TickParticleCS->SetSpawnBuffer(SpawnBuffer);
	TickParticleCS->SetParticleBuffer(ParticleBuffer);
	TickParticleCS->SetModuleBuffer(ModuleBuffer);

	TickParticleCS->Execute_Immediately();
}

void FNiagaraEmitter::Render() const
{
	if (RenderProperty)
	{
		ParticleBuffer->Binding(EffectBufferRegNum);
		RenderProperty->Render();
	}
}

std::shared_ptr<FNiagaraEmitter> FNiagaraEmitter::GetEmitterInstance() const
{
	auto Instance            = std::make_shared<FNiagaraEmitter>();
	Instance->Module         = Module;
	Instance->RenderProperty = RenderProperty;
	return Instance;
}

void FNiagaraEmitter::CalcSpawnCount(float DeltaSeconds)
{
	AccTime += DeltaSeconds;
	float Term = 1.f / Module.SpawnRate;

	FParticleSpawn Count{};
	if (bFirstTick)
	{
		Count.SpawnCount = 1;
		bFirstTick       = false;
	}
	if (AccTime >= Term)
	{
		AccTime -= Term;
		Count.SpawnCount = 1;
	}

	if (Module.Module[static_cast<int>(EParticleModule::PM_SpawnBurst)] && 0 < Module.SpawnBurstRepeat)
	{
		Module.AccSpawnBurstRepeatTime += DeltaSeconds;
		if (Module.SpawnBurstRepeatTime < Module.AccSpawnBurstRepeatTime)
		{
			Count.SpawnCount += Module.SpawnBurstCount;
			Module.SpawnBurstRepeat -= 1;
			Module.AccSpawnBurstRepeatTime -= Module.SpawnBurstRepeatTime;
		}
	}

	if (0 < Count.SpawnCount)
	{
		SpawnBuffer->SetData(&Count);
	}
}

void FNiagaraEmitter::LoadDataFromFile(const nlohmann::basic_json<>& Data)
{
	int PropertyType = Data["Property"];
	switch (PropertyType)
	{
	// 0: BillboardSprite
	case 0:
		RenderProperty = std::make_shared<FNiagaraRendererBillboardSprites>();
		break;
	// 1 : Sprite
	case 1:
		RenderProperty = std::make_shared<FNiagaraRendererSprites>();
		break;
	// 2 : Mesh
	case 2:
		RenderProperty = std::make_shared<FNiagaraRendererMeshes>();

		break;
	// 3: Ribbon
	case 3:
		RenderProperty = std::make_shared<FNiagaraRendererRibbons>();
		break;

	default: assert(0 && "잘못된 PropertyType");
		break;
	}
	RenderProperty->LoadDataFromFile(Data);

	Module.LoadDataFromFile(Data);
}

FNiagaraRibbonEmitter::FNiagaraRibbonEmitter()
{
	D3D11_BUFFER_DESC BufferDesc = {};
	BufferDesc.Usage             = D3D11_USAGE_DYNAMIC;
	// 한개의 리본 포인트 점당 6개의 버텍스가 생기므로
	BufferDesc.ByteWidth      = sizeof(MyVertexData) * MaxRibbonPointCount * 6;
	BufferDesc.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HR(GDirectXDevice->GetDevice()->CreateBuffer(&BufferDesc, nullptr, VB_Ribbon.GetAddressOf()));

	bUpdateAndRenderAtDeactivate = true;
}

std::shared_ptr<FNiagaraEmitter> FNiagaraRibbonEmitter::GetEmitterInstance() const
{
	auto Instance = std::make_shared<FNiagaraRibbonEmitter>();
	// 부모의 데이터를 그대로 복사해줘야함
	Instance->Module         = Module;
	Instance->RenderProperty = RenderProperty;
	Instance->RibbonWidth    = RibbonWidth;
	Instance->bIsBillboard   = bIsBillboard;
	Instance->RibbonColor    = RibbonColor;
	return Instance;
}

void FNiagaraRibbonEmitter::CreateAndAddNewRibbonPoint(XMFLOAT3 PointPos, XMVECTOR PointRot)
{
	// 최대 갯수를 넘으면 못만들게 설정
	if (CurPointCount >= MaxRibbonPointCount)
	{
		return;
	}

	// 새로운 데이터 생성
	FRibbonPointData Data;
	Data.PointPos   = PointPos;
	Data.RemainTime = Module.MaxLife;

	XMVECTOR Center    = XMLoadFloat3(&PointPos);
	float    HalfWidth = RibbonWidth * 0.5f;
	XMVECTOR UpPoint, DownPoint;
	if (bIsBillboard)
	{
		// TODO: 05/09, 렌더쓰레드의 ViewMatrix를 갖고올 좋은 방법이 생각 안나서 SceneData자체를 갖고와서 사용
		// 추후 가능하다면 전역변수로 바꿔 관리하거나 좋은 방법을 생각해보기
		XMMATRIX ViewMat        = FRenderCommandExecutor::CurrentSceneData->GetViewMatrix();
		XMMATRIX InvViewMat     = XMMatrixInverse(nullptr, ViewMat);
		XMVECTOR CameraFrontVec = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), XMQuaternionRotationMatrix(InvViewMat));
		XMVECTOR DeltaVec       = XMVector3Normalize(Center - LastFrameWorldPos);
		XMVECTOR NewPointVec    = XMVector3Normalize(XMVector3Cross(DeltaVec, CameraFrontVec));

		UpPoint   = Center + NewPointVec * HalfWidth;
		DownPoint = Center + NewPointVec * -HalfWidth;
	}
	else
	{
		XMVECTOR UpVec   = XMVectorSet(0, HalfWidth, 0, 0);
		XMVECTOR DownVec = XMVectorSet(0, -HalfWidth, 0, 0);

		XMVECTOR RotatedUp   = XMVector3Rotate(UpVec, PointRot);
		XMVECTOR RotatedDown = XMVector3Rotate(DownVec, PointRot);

		UpPoint   = Center + RotatedUp;
		DownPoint = Center + RotatedDown;
	}

	XMStoreFloat3(&Data.UpPointPos, UpPoint);
	XMStoreFloat3(&Data.DownPointPos, DownPoint);

	// 새로운 점 데이터 추가
	int NewDataIndex              = (CurRibbonPointDataStartIndex + CurPointCount) % MaxRibbonPointCount;
	RibbonPointData[NewDataIndex] = Data;
	++CurPointCount;
}

void FNiagaraRibbonEmitter::Tick(float DeltaSeconds, const FTransform& SceneTransform)
{
	XMFLOAT3 CurLoc         = SceneTransform.GetTranslation();
	XMVECTOR CurLocationVec = XMLoadFloat3(&CurLoc);

	// 시간 줄이기 + 시간이 다 된 점 삭제
	// 이 때, 항상 현재의 시작 타겟 인덱스부터 시간이 적은 순으로 배치되어 있으므로
	// 일단 현재 활성화된 점들에 대해서 dt만큼 줄여준다음에
	// 시간이 0 이하일경우 해당 자리는 그대로 냅두고(데이터를 지울 필요가 굳이 없음 (현재 활성화중인 개수를 따로 관리하니까)
	// 시작위치를 1칸 뒤로 보내주고 활성화된 점 개수도 1개 줄여줌
	for (int Count = 0; Count < CurPointCount; ++Count)
	{
		int CurIndex = (Count + CurRibbonPointDataStartIndex) % MaxRibbonPointCount;
		RibbonPointData[CurIndex].RemainTime -= DeltaSeconds;
		// 시간이 다 된 포인트면 활성화 개수 1개 줄이기
		if (RibbonPointData[CurIndex].RemainTime <= 0.0f)
		{
			CurPointCount -= 1;
			CurRibbonPointDataStartIndex = (CurRibbonPointDataStartIndex + 1) % MaxRibbonPointCount;
		}
	}

	// 위치 정보가 변경되었다면 새로운 점 추가
	// 05.13 : 리본렌더러가 Deactivate 되었을 때에는 궤적은 그대로 남아있어야함, 따라서 새로운 점을 생성하지 못하게만 제어
	if (Module.ActivateState == 0)
	{
		// 첫 프레임에는 위치를 고정해주고 해당위치에 포인트 생성
		if (bFirstTick)
		{
			bFirstTick        = false;
			LastFrameWorldPos = CurLocationVec;
		}

		float LocationDelta = XMVectorGetX(XMVector3LengthSq(XMVectorSubtract(CurLocationVec, LastFrameWorldPos)));
		if (LocationDelta > 0.1f)
		{
			CreateAndAddNewRibbonPoint(CurLoc, SceneTransform.GetRotationQuat());
			LastFrameWorldPos = CurLocationVec;
		}
	}

	// 새로운 데이터를 버텍스 버퍼에 Map 해주기
	MapPointDataToVertexBuffer();
}

void FNiagaraRibbonEmitter::Render() const
{
	if (CurPointCount < 2)
	{
		return;
	}

	GDirectXDevice->SetDSState(EDepthStencilStateType::NO_WRITE);
	GDirectXDevice->SetBSState(EBlendStateType::BST_AlphaBlend);

	auto DeviceContext = GDirectXDevice->GetDeviceContext();
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	const std::vector<std::shared_ptr<UTexture>>& OverrideTex = RenderProperty->GetTextureData();
	size_t                                        TextureSize = OverrideTex.size();
	for (int i = 0; i < TextureSize; ++i)
	{
		DeviceContext->PSSetShaderResources(i, 1, OverrideTex[i]->GetSRV().GetAddressOf());
	}

	UINT MeshIndex = 0;
	// 셰이더 설정
	UINT stride = sizeof(MyVertexData);
	UINT offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, VB_Ribbon.GetAddressOf(), &stride, &offset);

	DeviceContext->Draw(CurVertexCount, 0);

	GDirectXDevice->SetDSState(EDepthStencilStateType::LESS);
}

void FNiagaraRibbonEmitter::LoadDataFromFile(const nlohmann::basic_json<>& Data)
{
	FNiagaraEmitter::LoadDataFromFile(Data);

	float RibbonWidth = 5.0f;
	if (Data.contains("RibbonWidth"))
	{
		RibbonWidth = Data["RibbonWidth"];
	}
	SetRibbonWidth(RibbonWidth);
	if (Data.contains("FaceCamera"))
	{
		int bFaceCamera = Data["FaceCamera"];
		SetRibbonFaceCamera(bFaceCamera);
	}
	if (Data.contains("RibbonColor"))
	{
		const auto& RibbonColor = Data["RibbonColor"];
		SetRibbonColor(XMFLOAT4{RibbonColor[0], RibbonColor[1], RibbonColor[2], RibbonColor[3]});
	}
}

void FNiagaraRibbonEmitter::MapPointDataToVertexBuffer()
{
	// 버텍스 버퍼 내 포인트 개수랑 다른 경우에는 버텍스 버퍼를 갱신해주기
	if (CurPointCount == CurVertexBufferPointCount)
	{
		return;
	}

	// 버텍스 버퍼를 동적으로 갱신
	D3D11_MAPPED_SUBRESOURCE    cbMapSub{};
	ComPtr<ID3D11DeviceContext> DeviceContext = GDirectXDevice->GetDeviceContext();
	HR(DeviceContext->Map(VB_Ribbon.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &cbMapSub));

	CurVertexCount = 0;
	for (int i = 0; i < CurPointCount - 1; ++i)
	{
		int idx0 = (CurRibbonPointDataStartIndex + i) % MaxRibbonPointCount;
		int idx1 = (CurRibbonPointDataStartIndex + i + 1) % MaxRibbonPointCount;

		const FRibbonPointData& P0 = RibbonPointData[idx0];
		const FRibbonPointData& P1 = RibbonPointData[idx1];

		const int UV_X = static_cast<int>(1.0f / CurPointCount * i);

		// 네 점
		MyVertexData vA;
		vA.Pos       = {P0.UpPointPos};
		vA.TexCoords = {static_cast<float>(1) / CurPointCount * i, 0};
		MyVertexData vB;
		vB.Pos       = {P0.DownPointPos};
		vB.TexCoords = {static_cast<float>(1) / CurPointCount * i, 1};
		MyVertexData vC;
		vC.Pos       = {P1.UpPointPos};
		vC.TexCoords = {static_cast<float>(1) / CurPointCount * (i + 1), 0};
		MyVertexData vD;
		vD.Pos       = {P1.DownPointPos};
		vD.TexCoords = {static_cast<float>(1) / CurPointCount * (i + 1), 1};

		// 파티클 컬러 정보를 float4인 BONEWEIGHT 시맨틱에 담음
		float ParticleColor[4] = {RibbonColor.x, RibbonColor.y, RibbonColor.z, RibbonColor.w};
		for (int i = 0; i < 4; ++i)
		{
			vA.m_Weights[i] = ParticleColor[i];
			vB.m_Weights[i] = ParticleColor[i];
			vC.m_Weights[i] = ParticleColor[i];
			vD.m_Weights[i] = ParticleColor[i];
		}

		// 삼각형 1: A, B, C
		memcpy(static_cast<char*>(cbMapSub.pData) + sizeof(MyVertexData) * CurVertexCount++, &vA, sizeof(MyVertexData));
		memcpy(static_cast<char*>(cbMapSub.pData) + sizeof(MyVertexData) * CurVertexCount++, &vB, sizeof(MyVertexData));
		memcpy(static_cast<char*>(cbMapSub.pData) + sizeof(MyVertexData) * CurVertexCount++, &vC, sizeof(MyVertexData));
		// 삼각형 2: C, B, D
		memcpy(static_cast<char*>(cbMapSub.pData) + sizeof(MyVertexData) * CurVertexCount++, &vC, sizeof(MyVertexData));
		memcpy(static_cast<char*>(cbMapSub.pData) + sizeof(MyVertexData) * CurVertexCount++, &vB, sizeof(MyVertexData));
		memcpy(static_cast<char*>(cbMapSub.pData) + sizeof(MyVertexData) * CurVertexCount++, &vD, sizeof(MyVertexData));
	}

	DeviceContext->Unmap(VB_Ribbon.Get(), 0);

	CurVertexBufferPointCount = CurPointCount;
}
