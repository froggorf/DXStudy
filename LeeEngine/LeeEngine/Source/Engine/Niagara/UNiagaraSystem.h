// 05.04
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "CoreMinimal.h"

#include "FNiagaraEmitter.h"
#include "Engine/MyEngineUtils.h"
#include "Engine/UObject/UObject.h"

class UNiagaraSystem : public UObject
{
	MY_GENERATED_BODY(UNiagaraSystem)
public:
	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

	std::vector<std::shared_ptr<FNiagaraEmitter>> CreateDynamicRenderData() const;
protected:
private:
public:
protected:
private:
	std::vector<std::shared_ptr<FNiagaraEmitter>> Emitters;
};
