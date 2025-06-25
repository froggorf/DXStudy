// 02.16
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#include "CoreMinimal.h"
#include "UStaticMeshComponent.h"

#include "Engine/Mesh/UStaticMesh.h"
#include "Engine/Physics/UConvexComponent.h"
#include "Engine/RenderCore/EditorScene.h"
#include "Engine/SceneProxy/FStaticMeshSceneProxy.h"

UStaticMeshComponent::UStaticMeshComponent()
{
	Rename("StaticMeshComponent_" + std::to_string(ComponentID));

	
	
}

void UStaticMeshComponent::Register()
{
	UMeshComponent::Register();

	ConvexComponent = std::make_shared<UConvexComponent>();
	ConvexComponent->SetupAttachment(shared_from_this(),"");	
	//if (std::shared_ptr<USceneComponent> ThisComp = shared_from_this())
	//{
	//	
	//}

	if (ConvexComponent)
	{
		ConvexComponent->Register();	
	}
	
}

void UStaticMeshComponent::UnRegister()
{
	UMeshComponent::UnRegister();

	if (ConvexComponent)
	{
		ConvexComponent->UnRegister();	
	}
	
}

std::vector<std::shared_ptr<FPrimitiveSceneProxy>> UStaticMeshComponent::CreateSceneProxy()
{
	if(nullptr == StaticMesh)
	{
		return {};
	}

	std::vector<std::shared_ptr<FPrimitiveSceneProxy>> SceneProxies;

	
	UINT MeshCount = StaticMesh->GetStaticMeshRenderData()->MeshCount;
	SceneProxies.reserve(MeshCount);
	for (UINT i = 0; i < MeshCount; ++i)
	{
		auto SceneProxy = std::make_shared<FStaticMeshSceneProxy>(PrimitiveID, i, StaticMesh);
		SceneProxies.emplace_back(SceneProxy);
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
	ConvexComponent->SetStaticMesh(StaticMesh);
	
	// 새로운 씬 프록시가 등록될 수 있도록 진행
	RegisterSceneProxies();

	// 새로운 씬 프록시들을 생성 및 등록
	return true;
}
