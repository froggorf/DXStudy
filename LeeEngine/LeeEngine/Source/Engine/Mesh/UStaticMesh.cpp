// 02.14
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "UStaticMesh.h"

#include "StaticMeshResources.h"
#include "Engine/RenderCore/EditorScene.h"

//std::map<std::string, std::shared_ptr<UStaticMesh>> UStaticMesh::StaticMeshCache;

UStaticMesh::UStaticMesh()
{
}

UStaticMesh::~UStaticMesh()
{
}

const std::shared_ptr<UStaticMesh>& UStaticMesh::GetStaticMesh(const std::string& StaticMeshName)
{
	return std::dynamic_pointer_cast<UStaticMesh>(AssetManager::GetAssetCacheByName(StaticMeshName));
}

void UStaticMesh::LoadDataFromFileData(const nlohmann::json& StaticMeshAssetData)
{

	UObject::LoadDataFromFileData(StaticMeshAssetData);

	RenderData = std::make_unique<FStaticMeshRenderData>(StaticMeshAssetData);

	MY_LOG("Load", EDebugLogLevel::DLL_Display, "Load UStaticMesh - "+GetName() + " Complete!");
}
