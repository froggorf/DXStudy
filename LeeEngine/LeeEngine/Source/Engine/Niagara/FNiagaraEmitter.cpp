#include "CoreMinimal.h"
#include "FNiagaraEmitter.h"

#include "Engine/Mesh/UStaticMesh.h"
#include "Engine/SceneProxy/FNiagaraSceneProxy.h"

// ================= Niagara Renderer ================
void FNiagaraRendererBillboardSprites::Render()
{
	GDirectXDevice->SetDSState(EDepthStencilStateType::DST_NO_WRITE);
	GDirectXDevice->SetBSState(EBlendStateType::BST_AlphaBlend);
	auto DeviceContext = GDirectXDevice->GetDeviceContext();

	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	if(OverrideSpriteTexture)
	{
		DeviceContext->PSSetShaderResources(0,1,OverrideSpriteTexture->GetSRV().GetAddressOf()); 
	}


	auto RenderData = StaticMesh->GetStaticMeshRenderData();
	UINT MeshIndex = 0;
	// 셰이더 설정
	UINT stride = sizeof(MyVertexData);
	UINT offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, RenderData->VertexBuffer[MeshIndex].GetAddressOf(), &stride, &offset);
	DeviceContext->IASetIndexBuffer(RenderData->IndexBuffer[MeshIndex].Get(), DXGI_FORMAT_R32_UINT, 0);

	D3D11_BUFFER_DESC indexBufferDesc;
	RenderData->IndexBuffer[MeshIndex]->GetDesc(&indexBufferDesc);
	UINT indexSize = indexBufferDesc.ByteWidth / sizeof(UINT);
	DeviceContext->DrawIndexedInstanced(1,MaxParticleCount,0,0, 0);

	GDirectXDevice->SetDSState(EDepthStencilStateType::DST_LESS);
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void FNiagaraRendererSprites::Render()
{
	// 머테리얼만 다른 머테리얼을 사용
	FNiagaraRendererBillboardSprites::Render();
}

void FNiagaraRendererMeshes::Render()
{
	GDirectXDevice->SetDSState(EDepthStencilStateType::DST_NO_WRITE);
	GDirectXDevice->SetBSState(EBlendStateType::BST_AlphaBlend);

	auto DeviceContext = GDirectXDevice->GetDeviceContext();
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	size_t TextureSize = Textures.size();
	for(int i = 0 ; i < TextureSize; ++i)
	{
		DeviceContext->PSSetShaderResources(i,1,Textures[i]->GetSRV().GetAddressOf());
	}


	auto RenderData = BaseStaticMesh->GetStaticMeshRenderData();
	UINT MeshIndex = 0;
	// 셰이더 설정
	UINT stride = sizeof(MyVertexData);
	UINT offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, RenderData->VertexBuffer[MeshIndex].GetAddressOf(), &stride, &offset);
	DeviceContext->IASetIndexBuffer(RenderData->IndexBuffer[MeshIndex].Get(), DXGI_FORMAT_R32_UINT, 0);

	D3D11_BUFFER_DESC indexBufferDesc;
	RenderData->IndexBuffer[MeshIndex]->GetDesc(&indexBufferDesc);
	UINT indexSize = indexBufferDesc.ByteWidth / sizeof(UINT);
	DeviceContext->DrawIndexedInstanced(indexSize,MaxParticleCount,0,0, 0);

	GDirectXDevice->SetDSState(EDepthStencilStateType::DST_LESS);
}

void FNiagaraRendererRibbons::Render()
{
	
}
// ===========================================================================================

// ==================================== Niagara Emitter ========================================

std::shared_ptr<FTickParticleCS> FNiagaraEmitter::TickParticleCS;

FNiagaraEmitter::FNiagaraEmitter()
{
	
	Module.SpawnShape = 0;

	if(nullptr == TickParticleCS)
	{
		TickParticleCS = std::make_shared<FTickParticleCS>();	
	}

	ParticleBuffer = std::make_shared<FStructuredBuffer>();
	ParticleBuffer->Create(sizeof(FParticleData), MaxParticleCount, SB_TYPE::SRV_UAV, false);
	SpawnBuffer = std::make_shared<FStructuredBuffer>();
	SpawnBuffer->Create(sizeof(FParticleSpawn), 1, SB_TYPE::SRV_UAV, true);

	/**/

	ModuleBuffer = std::make_shared<FStructuredBuffer>();
	ModuleBuffer->Create(sizeof(FParticleModule), 1, SB_TYPE::SRV_ONLY, true,&Module);

	AccTime = 0;
	
}

void FNiagaraEmitter::Tick(float DeltaSeconds, const FTransform& SceneTransform)
{
	Module.ObjectWorldPos = SceneTransform.GetTranslation();

	// 이번 프레임에 활성화 될 파티클 수 계산
	CalcSpawnCount(DeltaSeconds);

	ModuleBuffer->SetData(&Module);
	TickParticleCS->SetSpawnBuffer(SpawnBuffer);
	TickParticleCS->SetParticleBuffer(ParticleBuffer);
	TickParticleCS->SetModuleBuffer(ModuleBuffer);

	TickParticleCS->Execute_Immediately();
}

void FNiagaraEmitter::Render() const
{
	if(RenderData)
	{
		ParticleBuffer->Binding(20);

		RenderData->Render();
	}
}

std::shared_ptr<FNiagaraEmitter> FNiagaraEmitter::GetEmitterInstance() const
{
	std::shared_ptr<FNiagaraEmitter> Instance = std::make_shared<FNiagaraEmitter>();
	Instance->Module = Module;
	Instance->RenderData = RenderData;
	return Instance;
}

void FNiagaraEmitter::CalcSpawnCount(float DeltaSeconds)
{
	AccTime += DeltaSeconds;
	float Term = 1.f / Module.SpawnRate;

	FParticleSpawn Count{};
	if(bFirstTick)
	{
		Count.SpawnCount = 1;
		bFirstTick = false;
	}
	if (AccTime >= Term)
	{
		AccTime -= Term;
		Count.SpawnCount = 1;
	}

	if(Module.Module[static_cast<int>(EParticleModule::PM_SPAWN_BURST)] && 0 < Module.SpawnBurstRepeat)
	{
		Module.AccSpawnBurstRepeatTime += DeltaSeconds;
		if(Module.SpawnBurstRepeatTime < Module.AccSpawnBurstRepeatTime)
		{
			Count.SpawnCount += Module.SpawnBurstCount;
			Module.SpawnBurstRepeat -= 1;
			Module.AccSpawnBurstRepeatTime-= Module.SpawnBurstRepeatTime;
		}
	}

	if(0< Count.SpawnCount)
	{
		SpawnBuffer->SetData(&Count);
	}
}

FNiagaraRibbonEmitter::FNiagaraRibbonEmitter()
{
	D3D11_BUFFER_DESC BufferDesc = {};
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	// 한개의 리본 포인트 점당 6개의 버텍스가 생기므로
	BufferDesc.ByteWidth = sizeof(MyVertexData) * MaxRibbonPointCount * 6;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HR(GDirectXDevice->GetDevice()->CreateBuffer(&BufferDesc, nullptr, VB_Ribbon.GetAddressOf()));
}

std::shared_ptr<FNiagaraEmitter> FNiagaraRibbonEmitter::GetEmitterInstance() const
{
	std::shared_ptr<FNiagaraRibbonEmitter> Instance = std::make_shared<FNiagaraRibbonEmitter>();
	// 부모의 데이터를 그대로 복사해줘야함
	Instance->Module = Module;
	Instance->RenderData = RenderData;
	Instance->RibbonWidth = RibbonWidth;
	Instance->bIsBillboard = bIsBillboard;
	Instance->RibbonColor = RibbonColor;
	return Instance;
}

void FNiagaraRibbonEmitter::CreateAndAddNewRibbonPoint(XMFLOAT3 PointPos, XMVECTOR PointRot)
{
	// 최대 갯수를 넘으면 못만들게 설정
	if(CurPointCount >= MaxRibbonPointCount)
	{
		return;
	}
	
	// 새로운 데이터 생성
	FRibbonPointData Data;
	Data.PointPos = PointPos;
	Data.RemainTime = Module.MaxLife;


	XMVECTOR Center = XMLoadFloat3(&PointPos);
	float HalfWidth = RibbonWidth * 0.5f;
	XMVECTOR UpPoint, DownPoint;
	if(bIsBillboard)
	{
		// TODO: 05/09, 렌더쓰레드의 ViewMatrix를 갖고올 좋은 방법이 생각 안나서 SceneData자체를 갖고와서 사용
		// 추후 가능하다면 전역변수로 바꿔 관리하거나 좋은 방법을 생각해보기
		XMMATRIX ViewMat = FRenderCommandExecutor::CurrentSceneData->GetViewMatrix();
		XMMATRIX InvViewMat = XMMatrixInverse(nullptr, ViewMat);
		XMVECTOR CameraFrontVec = XMVector3Rotate(XMVectorSet(0,0,1,0), XMQuaternionRotationMatrix(InvViewMat));
		XMVECTOR DeltaVec = XMVector3Normalize(Center - LastFrameWorldPos);
		XMVECTOR NewPointVec = XMVector3Normalize(XMVector3Cross(DeltaVec, CameraFrontVec));;

		UpPoint = Center + NewPointVec * HalfWidth;
		DownPoint = Center + NewPointVec * -HalfWidth;

	}
	else
	{
		XMVECTOR UpVec = XMVectorSet(0, HalfWidth, 0, 0);
		XMVECTOR DownVec = XMVectorSet(0, -HalfWidth, 0, 0);

		XMVECTOR RotatedUp = XMVector3Rotate(UpVec, PointRot);
		XMVECTOR RotatedDown = XMVector3Rotate(DownVec, PointRot);

		UpPoint = Center + RotatedUp;
		DownPoint = Center + RotatedDown;
	}

	XMStoreFloat3(&Data.UpPointPos, UpPoint);
	XMStoreFloat3(&Data.DownPointPos, DownPoint);

	// 새로운 점 데이터 추가
	int NewDataIndex = (CurRibbonPointDataStartIndex + CurPointCount) % MaxRibbonPointCount;
	// array의 최대치를 넘김
	RibbonPointData[NewDataIndex] = Data;
	++CurPointCount;

	
}

void FNiagaraRibbonEmitter::Tick(float DeltaSeconds, const FTransform& SceneTransform)
{
	XMFLOAT3 CurLoc = SceneTransform.GetTranslation();
	XMVECTOR CurLocationVec = XMLoadFloat3(&CurLoc);

	// 시간 줄이기 + 시간이 다 된 점 삭제
	// 이 때, 항상 현재의 시작 타겟 인덱스부터 시간이 적은 순으로 배치되어 있으므로
	// 일단 현재 활성화된 점들에 대해서 dt만큼 줄여준다음에
	// 시간이 0 이하일경우 해당 자리는 그대로 냅두고(데이터를 지울 필요가 굳이 없음 (현재 활성화중인 개수를 따로 관리하니까)
	// 시작위치를 1칸 뒤로 보내주고 활성화된 점 개수도 1개 줄여줌
	for(int Count = 0; Count < CurPointCount; ++Count)
	{
		int CurIndex = (Count + CurRibbonPointDataStartIndex) % MaxRibbonPointCount;
		RibbonPointData[CurIndex].RemainTime -= DeltaSeconds;
		// 시간이 다 된 포인트면 활성화 개수 1개 줄이기
		if(RibbonPointData[CurIndex].RemainTime <= 0.0f)
		{
			CurPointCount -=1;
			CurRibbonPointDataStartIndex = (CurRibbonPointDataStartIndex+1)%MaxRibbonPointCount;
		}
	}

	// 첫 프레임에는 위치를 고정해주고 해당위치에 포인트 생성
	// TODO : 5/8) 이 부분은 LastFrameWorldPos 를 엄청 멀리 두면 시작위치가 다르니까 적용되지 않을까 싶음
	if(bFirstTick)
	{
		bFirstTick = false;
		LastFrameWorldPos = CurLocationVec;
		CreateAndAddNewRibbonPoint(CurLoc, SceneTransform.GetRotationQuat());
	}

	// 위치 정보가 변경되었다면 새로운 점 추가
	float LocationDelta = XMVectorGetX(XMVector3LengthSq(XMVectorSubtract(CurLocationVec, LastFrameWorldPos)) );
	if(LocationDelta> 0.1f)
	{
		CreateAndAddNewRibbonPoint(CurLoc, SceneTransform.GetRotationQuat());
		LastFrameWorldPos = CurLocationVec;
	}

	// 새로운 데이터를 버텍스 버퍼에 Map 해주기
	MapPointDataToVertexBuffer();

}

void FNiagaraRibbonEmitter::Render() const
{
	if(CurPointCount < 2)
	{
		return;
	}

	GDirectXDevice->SetDSState(EDepthStencilStateType::DST_NO_WRITE);
	GDirectXDevice->SetBSState(EBlendStateType::BST_AlphaBlend);

	auto DeviceContext = GDirectXDevice->GetDeviceContext();
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//size_t TextureSize = Textures.size();
	//for(int i = 0 ; i < TextureSize; ++i)
	//{
	//	DeviceContext->PSSetShaderResources(i,1,Textures[i]->GetSRV().GetAddressOf());
	//}


	UINT MeshIndex = 0;
	// 셰이더 설정
	UINT stride = sizeof(MyVertexData);
	UINT offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, VB_Ribbon.GetAddressOf(), &stride, &offset);

	DeviceContext->Draw(CurVertexCount, 0);

	GDirectXDevice->SetDSState(EDepthStencilStateType::DST_LESS);
}

void FNiagaraRibbonEmitter::MapPointDataToVertexBuffer()
{
	// 버텍스 버퍼 내 포인트 개수랑 다른 경우에는 버텍스 버퍼를 갱신해주기
	if(CurPointCount == CurVertexBufferPointCount)
	{
		return;
	}

	// 갱신
	D3D11_MAPPED_SUBRESOURCE cbMapSub{};
	ComPtr<ID3D11DeviceContext> DeviceContext = GDirectXDevice->GetDeviceContext();
	HR(DeviceContext->Map(VB_Ribbon.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &cbMapSub));


	CurVertexCount = 0;
	for (int i = 0; i < CurPointCount - 1; ++i)
	{
		int idx0 = (CurRibbonPointDataStartIndex + i) % MaxRibbonPointCount;
		int idx1 = (CurRibbonPointDataStartIndex + i + 1) % MaxRibbonPointCount;

		const FRibbonPointData& P0 = RibbonPointData[idx0];
		const FRibbonPointData& P1 = RibbonPointData[idx1];

		const int UV_X = static_cast<float>(1) / CurPointCount * i;

		// 네 점
		MyVertexData vA; vA.Pos = { P0.UpPointPos };	vA.TexCoords = {static_cast<float>(1) / CurPointCount * i, 0};
		MyVertexData vB; vB.Pos = { P0.DownPointPos };	vB.TexCoords = {static_cast<float>(1) / CurPointCount * i, 1};
		MyVertexData vC; vC.Pos = { P1.UpPointPos };	vC.TexCoords = {static_cast<float>(1) / CurPointCount * (i+1), 0};
		MyVertexData vD; vD.Pos = { P1.DownPointPos };	vD.TexCoords = {static_cast<float>(1) / CurPointCount * (i+1), 1};

		// 파티클 컬러 정보를 float4인 BONEWEIGHT 시맨틱에 담음
		float ParticleColor[4] = {RibbonColor.x, RibbonColor.y, RibbonColor.z, RibbonColor.w};
		for(int i = 0; i < 4; ++i)
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
