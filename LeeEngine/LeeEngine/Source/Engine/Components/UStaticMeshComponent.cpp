// 02.16
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "UStaticMeshComponent.h"

#include "Engine/Mesh/UStaticMesh.h"


UStaticMeshComponent::UStaticMeshComponent()
{
	Rename("StaticMeshComponent_" + std::to_string(ComponentID));

}

bool UStaticMeshComponent::SetStaticMesh(const std::shared_ptr<UStaticMesh>& NewMesh)
{
	if(nullptr == NewMesh)
	{
		MY_LOG("SetStaticMesh", EDebugLogLevel::DLL_Warning, "nullptr StaticMesh");
		return false;
	}

	if(NewMesh.get() == GetStaticMesh().get())
	{
		return false;
	}

	StaticMesh = NewMesh;

	return true;
}

void UStaticMeshComponent::TestDraw()
{
	USceneComponent::TestDraw();
}

void UStaticMeshComponent::TestDrawComponent()
{
	USceneComponent::TestDrawComponent();

	{
		ObjConstantBuffer ocb;
		XMMATRIX world = GetComponentTransform().ToMatrixWithScale();
		// 조명 - 노말벡터의 변환을 위해 역전치 행렬 추가
		ocb.InvTransposeMatrix = (XMMatrixInverse(nullptr, world));
		ocb.World = XMMatrixTranspose(world);

		ocb.ObjectMaterial.Ambient  = XMFLOAT4(1.0f,1.0f,1.0f, 1.0f);
		ocb.ObjectMaterial.Diffuse  = XMFLOAT4(1.0f,1.0f,1.0f, 1.0f);
		ocb.ObjectMaterial.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 32.0f);

		GDirectXDevice->GetDeviceContext()->UpdateSubresource(GEngine->TestDeleteLater_GetObjConstantBuffer(), 0, nullptr, &ocb, 0, 0);
	}

	const FStaticMeshRenderData* RenderData = GetStaticMesh()->GetStaticMeshRenderData();
	unsigned int MeshCount = RenderData->MeshCount;
	for(int MeshIndex= 0; MeshIndex < MeshCount; ++MeshIndex)
	{
		ID3D11DeviceContext* DeviceContext = GDirectXDevice->GetDeviceContext().Get();
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
