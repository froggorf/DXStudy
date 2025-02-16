// 02.16
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "UStaticMeshComponent.h"

#include "Engine/Mesh/UStaticMesh.h"

UStaticMeshComponent::UStaticMeshComponent()
{
}

bool UStaticMeshComponent::SetStaticMesh(UStaticMesh* NewMesh)
{
	if(NewMesh == GetStaticMesh().get())
	{
		return false;
	}

	StaticMesh = std::shared_ptr<UStaticMesh>(NewMesh);

	return true;
}

void UStaticMeshComponent::TestDrawComponent()
{
	const FStaticMeshRenderData* RenderData = GetStaticMesh()->GetStaticMeshRenderData();
	int MeshCount = RenderData->MeshCount;
	for(int MeshIndex= 0; MeshIndex < MeshCount; ++MeshIndex)
	{
		ID3D11DeviceContext* DeviceContext = GEngine->GetDeviceContext();
		// SRV 설정(텍스쳐)
		{
			DeviceContext->PSSetShaderResources(0,1, RenderData->TextureSRV[0].GetAddressOf());	
		}
		UINT stride = sizeof(MyVertexData);
		UINT offset = 0;
		DeviceContext->IASetVertexBuffers(0, 1, RenderData->VertexBuffer[MeshIndex].GetAddressOf(), &stride, &offset);
		DeviceContext->IASetIndexBuffer(RenderData->IndexBuffer[MeshIndex].Get(), DXGI_FORMAT_R32_UINT, 0);
	
		D3D11_BUFFER_DESC indexBufferDesc;
		RenderData->IndexBuffer[MeshIndex]->GetDesc(&indexBufferDesc);
		UINT indexSize = indexBufferDesc.ByteWidth / sizeof(UINT);
		DeviceContext->DrawIndexed(indexSize, 0, 0);
	}
}
