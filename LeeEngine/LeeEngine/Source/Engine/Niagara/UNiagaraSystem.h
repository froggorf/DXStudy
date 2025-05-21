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
	MY_GENERATE_BODY(UNiagaraSystem)
	// 데이터 로드 함수
	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

	// 런타임에 NiagaraSystem의 인스턴스를 만들어 반환하는 함수
	std::vector<std::shared_ptr<FNiagaraEmitter>> CreateDynamicRenderData() const;

	// NiagaraSystem 에셋을 얻어오는 함수
	static std::shared_ptr<UNiagaraSystem> GetNiagaraAsset(const std::string& NiagaraAssetName);

protected:
	// Emitter 벡터
	// 언리얼엔진의 NiagaraSystem 내에 있는 하나의 이펙트 이미터 단위를 표현
	std::vector<std::shared_ptr<FNiagaraEmitter>> Emitters;

private:
};
