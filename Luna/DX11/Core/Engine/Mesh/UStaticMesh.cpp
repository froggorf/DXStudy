// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "UStaticMesh.h"

#include "d3dApp.h"
#include "StaticMeshResources.h"

UStaticMesh::UStaticMesh(const std::vector<std::string>& StaticMeshAssetData, const ComPtr<ID3D11Device>& DeviceObject)
{
	RenderData = std::make_unique<FStaticMeshRenderData>(StaticMeshAssetData, DeviceObject);

	std::cout<< "Load UStaticMesh - " + StaticMeshAssetData[0] + " Complete!" << std::endl;
}

UStaticMesh::~UStaticMesh()
{
}
