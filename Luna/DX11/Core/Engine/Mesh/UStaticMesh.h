// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "StaticMeshResources.h"
#include "Engine/MyEngineUtils.h"
#include "Engine/UEngine.h"
#include "Engine/UObject/UObject.h"

class UStaticMesh : public UObject
{
public:
	UStaticMesh(const std::vector<std::string>& StaticMeshAssetData, const Microsoft::WRL::ComPtr<ID3D11Device>& DeviceObject);
	~UStaticMesh();
	UStaticMesh(const UStaticMesh& other)
	{
		RenderData = std::make_unique<FStaticMeshRenderData>(*other.RenderData.get());
	}

	unsigned int GetStaticMeshMeshCount() const {return RenderData.get()->MeshCount; }

	void TestDraw()
	{
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
protected:
private:

public:
protected:
private:
	// TODO: LOD 데이터가 필요한 경우 std::map<UINT, std::unique_ptr<FStaticMeshRenderData> LODRenderData; 로 변경 예정
	// StaticMesh의 버텍스, 인덱스 버퍼 등 렌더링에 필요한 데이터를 관리
	std::unique_ptr<FStaticMeshRenderData> RenderData;
	
};
