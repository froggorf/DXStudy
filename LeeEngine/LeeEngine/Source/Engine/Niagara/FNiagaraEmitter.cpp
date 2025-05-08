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

Microsoft::WRL::ComPtr<ID3D11Buffer> FNiagaraRibbonEmitter::VB_Ribbon;

FNiagaraRibbonEmitter::FNiagaraRibbonEmitter()
{
	if(nullptr == VB_Ribbon)
	{
		D3D11_BUFFER_DESC BufferDesc = {};
		BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		BufferDesc.ByteWidth = sizeof(MyVertexData) * MaxParticleCount;
		BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		HR(GDirectXDevice->GetDevice()->CreateBuffer(&BufferDesc, nullptr, VB_Ribbon.GetAddressOf()));
	}
}

std::shared_ptr<FNiagaraEmitter> FNiagaraRibbonEmitter::GetEmitterInstance() const
{
	std::shared_ptr<FNiagaraRibbonEmitter> Instance = std::make_shared<FNiagaraRibbonEmitter>();
	// 부모의 데이터를 그대로 복사해줘야함
	Instance->Module = Module;
	Instance->RenderData = RenderData;
	Instance->RibbonWidth = RibbonWidth;
	return Instance;
}

void FNiagaraRibbonEmitter::Tick(float DeltaSeconds, const FTransform& SceneTransform)
{
	// 시간이 다 된 점 삭제
	// 위치 정보가 변경되었다면 새로운 점 추가
	// LastFrameWorldPos 와 SceneTransform의 월드 포즈가 변경되었다면,

}

void FNiagaraRibbonEmitter::Render() const
{
	if(RenderData)
	{
		RenderData->Render();
	}
}
