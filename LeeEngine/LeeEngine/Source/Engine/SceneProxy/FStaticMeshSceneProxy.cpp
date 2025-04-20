// 03.10
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "FStaticMeshSceneProxy.h"

#include "Engine/UEditorEngine.h"
#include "Engine/Mesh/StaticMeshResources.h"
#include "Engine/Mesh/UStaticMesh.h"
#include "Engine/RenderCore/EditorScene.h"


FStaticMeshSceneProxy::FStaticMeshSceneProxy(UINT InPrimitiveID, UINT InMeshIndex,
	const std::shared_ptr<UStaticMesh>& StaticMesh)
	: FPrimitiveSceneProxy(InPrimitiveID)
{
	MeshIndex = InMeshIndex;
	RenderData = StaticMesh->GetStaticMeshRenderData();
	MaterialInterface = RenderData->Materials[MeshIndex];

	std::string Text = "FStaticMeshSceneProxy Create StaticMeshSceneProxy - " + std::to_string(InPrimitiveID);
	MY_LOG(Text, EDebugLogLevel::DLL_Display, "");
}

FStaticMeshSceneProxy::~FStaticMeshSceneProxy()
{
}

void FStaticMeshSceneProxy::Draw()
{
	if(!RenderData)
	{
		return;
	}

	FPrimitiveSceneProxy::Draw();


	ID3D11DeviceContext* DeviceContext = GDirectXDevice->GetDeviceContext().Get();




	// 셰이더 설정
	UINT stride = sizeof(MyVertexData);
	UINT offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, RenderData->VertexBuffer[MeshIndex].GetAddressOf(), &stride, &offset);
	DeviceContext->IASetIndexBuffer(RenderData->IndexBuffer[MeshIndex].Get(), DXGI_FORMAT_R32_UINT, 0);

	D3D11_BUFFER_DESC indexBufferDesc;
	RenderData->IndexBuffer[MeshIndex]->GetDesc(&indexBufferDesc);
	UINT indexSize = indexBufferDesc.ByteWidth / sizeof(UINT);
	DeviceContext->DrawIndexed(indexSize, 0, 0);
	
}
