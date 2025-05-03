#include "CoreMinimal.h"
#include "FNiagaraSceneProxy.h"

#include <stdbool.h>

#include "Engine/Mesh/UStaticMesh.h"

FNiagaraSceneProxy::FNiagaraSceneProxy(UINT InPrimitiveID)
	: FPrimitiveSceneProxy(InPrimitiveID)
{
	MaterialInterface = UMaterial::GetMaterialCache("M_NiagaraBillboardSprite");
	ParticleBuffer = std::make_shared<FStructuredBuffer>();
	ParticleBuffer->Create(sizeof(FParticleData), MaxParticleCount, SB_TYPE::SRV_UAV, false);

}



void FNiagaraSceneProxy::Draw()
{
	FPrimitiveSceneProxy::Draw();

	
	UMaterial::GetMaterialCache("M_NiagaraBillboardSprite")->Binding();

	ParticleBuffer->Binding(20);

	auto DeviceContext = GDirectXDevice->GetDeviceContext();
	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	auto sm = UStaticMesh::GetStaticMesh("SM_Point");
	auto RenderData = sm->GetStaticMeshRenderData();
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





	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void FNiagaraSceneProxy::TickCS()
{

}
