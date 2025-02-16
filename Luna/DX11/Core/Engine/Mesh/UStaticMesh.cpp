// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "UStaticMesh.h"

#include "d3dApp.h"
#include "StaticMeshResources.h"


std::map<std::string, std::shared_ptr<UStaticMesh>> UStaticMesh::StaticMeshCache;

UStaticMesh::UStaticMesh()
{
	
}

UStaticMesh::~UStaticMesh()
{
}

UStaticMesh::UStaticMesh(const UStaticMesh& other)
{
	RenderData = std::make_unique<FStaticMeshRenderData>(*other.RenderData.get());
}


enum class EStaticMeshAssetData
{
	ESMAD_Name = 0, ESMAD_ModelDataFilePath, ESMAD_TextureDataFilePath
};

void UStaticMesh::LoadDataFromFileData(std::vector<std::string>& StaticMeshAssetData)
{
	UObject::LoadDataFromFileData(StaticMeshAssetData);


	const std::string Name = StaticMeshAssetData[static_cast<unsigned int>(EStaticMeshAssetData::ESMAD_Name)];
	StaticMeshAssetData.erase(StaticMeshAssetData.begin());

	if(StaticMeshCache.contains(Name))
	{
		std::cout << "already load this StaticMesh -> " << Name << std::endl;
		return;
	}

	RenderData = std::make_unique<FStaticMeshRenderData>(StaticMeshAssetData, GEngine->GetDevice());


	StaticMeshCache[Name] = std::make_shared<UStaticMesh>(*this);
	std::cout<< "Load UStaticMesh - " + StaticMeshAssetData[0] + " Complete!" << std::endl;
}
