// 02.16
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석
#pragma once
#include "UMeshComponent.h"
#include "Engine/MyEngineUtils.h"

class UStaticMesh;

class UStaticMeshComponent : public UMeshComponent
{
	MY_GENERATED_BODY(UStaticMeshComponent)
	UStaticMeshComponent();
	std::vector<std::shared_ptr<FPrimitiveSceneProxy>> CreateSceneProxy() override;
	virtual bool                                       SetStaticMesh(const std::shared_ptr<UStaticMesh>& NewMesh);

	const std::shared_ptr<UStaticMesh>& GetStaticMesh() const
	{
		return StaticMesh;
	}

private:
	std::shared_ptr<UStaticMesh> StaticMesh;
};
