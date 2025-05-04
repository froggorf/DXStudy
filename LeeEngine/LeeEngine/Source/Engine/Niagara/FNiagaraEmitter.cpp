#include "CoreMinimal.h"
#include "FNiagaraEmitter.h"

#include "Engine/Mesh/UStaticMesh.h"
#include "Engine/SceneProxy/FNiagaraSceneProxy.h"

void FNiagaraRendererBillboardSprites::Render()
{
	auto DeviceContext = GDirectXDevice->GetDeviceContext();

	if(OverrideSpriteTexture)
	{
		DeviceContext->PSSetShaderResources(0,1,OverrideSpriteTexture->GetSRV().GetAddressOf()); 
	}


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


}
