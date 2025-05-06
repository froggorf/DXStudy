#include "CoreMinimal.h"
#include "FNiagaraEmitter.h"

#include "Engine/Mesh/UStaticMesh.h"
#include "Engine/SceneProxy/FNiagaraSceneProxy.h"

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
