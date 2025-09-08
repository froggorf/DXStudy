// 08.13
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/Class/Light/ULightComponent.h"
#include "Engine/Components/UStaticMeshComponent.h"
#include "Engine/GameFramework/AActor.h"

class ASkyBox : public AActor
{
	MY_GENERATE_BODY(ASkyBox)
public:
	ASkyBox();
	void Tick(float DeltaSeconds) override;

#ifdef WITH_EDITOR
	void Tick_Editor(float DeltaSeconds) override;
#endif
private:

	std::shared_ptr<ULightComponent> DirectionalLight;
	std::shared_ptr<UStaticMeshComponent> SM_SkyBox;
	
};
