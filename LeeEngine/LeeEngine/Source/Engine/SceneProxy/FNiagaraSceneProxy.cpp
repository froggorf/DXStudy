#include "CoreMinimal.h"
#include "FNiagaraSceneProxy.h"

#include "Engine/Mesh/UStaticMesh.h"

FNiagaraSceneProxy::FNiagaraSceneProxy(UINT InPrimitiveID)
	: FPrimitiveSceneProxy(InPrimitiveID)
{
	MaterialInterface = UMaterial::GetMaterialCache("M_Cube");
}

void FNiagaraSceneProxy::Draw()
{
	FPrimitiveSceneProxy::Draw();
	auto DeviceContext = GDirectXDevice->GetDeviceContext();
	//DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	auto sm = UStaticMesh::GetStaticMesh("SM_Cube");
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
	DeviceContext->DrawIndexed(indexSize, 0, 0);





	DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
