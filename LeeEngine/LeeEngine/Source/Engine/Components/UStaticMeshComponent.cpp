// 02.16
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "UStaticMeshComponent.h"

#include "Engine/Mesh/UStaticMesh.h"
#include "Engine/RenderCore/EditorScene.h"
#include "Engine/SceneProxy/FStaticMeshSceneProxy.h"

UStaticMeshComponent::UStaticMeshComponent()
{
	Rename("StaticMeshComponent_" + std::to_string(ComponentID));
}

std::vector<std::shared_ptr<FPrimitiveSceneProxy>> UStaticMeshComponent::CreateSceneProxy()
{
	if(nullptr == StaticMesh)
	{
		return {};
	}

	std::vector<std::shared_ptr<FPrimitiveSceneProxy>> SceneProxies;

	
	UINT MeshCount = StaticMesh->GetStaticMeshRenderData()->MeshCount;
	if(MeshCount == 0)
	{
		int a = 0;
	}
	SceneProxies.reserve(MeshCount);
	StaticMeshSceneProxies.reserve(MeshCount);
	for (UINT i = 0; i < MeshCount; ++i)
	{
		auto SceneProxy = std::make_shared<FStaticMeshSceneProxy>(PrimitiveID, i, StaticMesh);
		SceneProxies.emplace_back(SceneProxy);
		StaticMeshSceneProxies.emplace_back(SceneProxy);
	}

	return SceneProxies;
}

bool UStaticMeshComponent::SetStaticMesh(const std::shared_ptr<UStaticMesh>& NewMesh)
{
	if (nullptr == NewMesh)
	{
		MY_LOG("SetStaticMesh", EDebugLogLevel::DLL_Warning, "nullptr StaticMesh");
		return false;
	}

	if (NewMesh.get() == GetStaticMesh().get())
	{
		return false;
	}


	StaticMesh = NewMesh;

	// 새로운 씬 프록시가 등록될 수 있도록 진행
	StaticMeshSceneProxies.clear();
	RegisterSceneProxies();

	// 새로운 씬 프록시들을 생성 및 등록
	return true;
}
