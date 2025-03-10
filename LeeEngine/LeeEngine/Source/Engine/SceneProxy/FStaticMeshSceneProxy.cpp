// 03.10
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#include "FStaticMeshSceneProxy.h"

#include "Engine/UEditorEngine.h"
FStaticMeshSceneProxy::FStaticMeshSceneProxy(UINT PrimitiveID, const std::shared_ptr<UStaticMesh>& StaticMesh)
	: FPrimitiveSceneProxy(PrimitiveID)
{
	
	std::string Text = "FStaticMeshSceneProxy Create StaticMeshSceneProxy - " + std::to_string(PrimitiveID);
	MY_LOG(Text, EDebugLogLevel::DLL_Display, "");
}

void FStaticMeshSceneProxy::Draw()
{
	
}
