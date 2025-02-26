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


enum class EStaticMeshAssetData
{
	ESMAD_Name = 0, ESMAD_ModelDataFilePath, ESMAD_TextureDataFilePath
};

void UStaticMesh::LoadDataFromFileData(const nlohmann::json& StaticMeshAssetData)
{
	if(StaticMeshCache.contains(GetName()))
	{
		MY_LOG("LoadData", EDebugLogLevel::DLL_Warning, "already load this StaticMesh -> " + GetName());
		return;
	}

	UObject::LoadDataFromFileData(StaticMeshAssetData);

	RenderData = std::make_unique<FStaticMeshRenderData>(StaticMeshAssetData, GEngine->GetDevice());

	StaticMeshCache[GetName()] = shared_from_this();

	MY_LOG("Load", EDebugLogLevel::DLL_Display, "Load UStaticMesh - "+GetName() + " Complete!");
}
