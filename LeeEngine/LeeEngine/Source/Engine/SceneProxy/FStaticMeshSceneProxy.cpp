// 03.10
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "FStaticMeshSceneProxy.h"
#include "Engine/Mesh/UStaticMesh.h"

FStaticMeshSceneProxy::FStaticMeshSceneProxy(UINT InPrimitiveID, UINT InMeshIndex, const std::shared_ptr<UStaticMesh>& StaticMesh)
	: FPrimitiveSceneProxy(InPrimitiveID)
{
	MeshIndex         = InMeshIndex;
	if(StaticMesh)
	{
		
		RenderData        = StaticMesh->GetStaticMeshRenderData();
		MaterialInterface = RenderData->Materials[MeshIndex];	
	}
	else
	{
#ifdef WITH_EDITOR
		std::string Temp = std::to_string(InPrimitiveID) + " - " + std::to_string(InMeshIndex) + "StaticMesh is not bind";
		MY_LOG("FStaticMeshSceneProxy - " , EDebugLogLevel::DLL_Warning, Temp);
#endif
	}
	

	
}

FStaticMeshSceneProxy::~FStaticMeshSceneProxy()
{
	
}

void FStaticMeshSceneProxy::SetNewRenderData(const std::shared_ptr<UStaticMesh>& NewStaticMesh)
{
	if(nullptr == NewStaticMesh)
	{
		MY_LOG("FStaticMeshSceneProxy - SetNewRenderData" , EDebugLogLevel::DLL_Warning, "No valid StaticMesh");
		return;
	}
	
	RenderData = NewStaticMesh->GetStaticMeshRenderData();

	// 머테리얼의 정보가 없다면 해당 정보도 채워주기
	if(!MaterialInterface)
	{
		// 머테리얼 정보가 없으면 생략
		if(RenderData->Materials.empty())
		{
			return;
		}

		UINT MaterialIndex = MeshIndex;
		// 만약 초과된 메쉬일경우 0번 머테리얼을 참조하도록
		if(MeshIndex >= static_cast<UINT>(RenderData->Materials.size()))
		{
			MaterialIndex = 0;
		}
		MaterialInterface = RenderData->Materials[MaterialIndex];
	}
}

void FStaticMeshSceneProxy::Draw()
{
	if (!RenderData || !RenderData->bLoadSuccess)
	{
		return;
	}	

	// 현재 렌더 데이터인 스태틱메시가
	// 내 FStaticMeshSceneProxy의 메쉬 인덱스보다 클 경우 그리면 안됨
	if(MeshIndex >= RenderData->VertexBuffer.size())
	{
		return;
	}

	ID3D11DeviceContext* DeviceContext = GDirectXDevice->GetDeviceContext().Get();

	FPrimitiveSceneProxy::Draw();

	UINT stride = sizeof(MyVertexData);
	UINT offset = 0;
	DeviceContext->IASetVertexBuffers(0, 1, RenderData->VertexBuffer[MeshIndex].GetAddressOf(), &stride, &offset);
	DeviceContext->IASetIndexBuffer(RenderData->IndexBuffer[MeshIndex].Get(), DXGI_FORMAT_R32_UINT, 0);
	
	UINT IndexCount = static_cast<UINT>(RenderData->IndexData[MeshIndex].size());
	DeviceContext->DrawIndexed(IndexCount, 0, 0);
	
}
