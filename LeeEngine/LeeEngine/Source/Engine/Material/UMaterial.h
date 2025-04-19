// 04.19
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once

#include "Engine/MyEngineUtils.h"
#include "Engine/UObject/UObject.h"




class FShader
{
protected:
	std::unordered_map<std::string, std::shared_ptr<FShader>> ShaderCache;	
};

class FGraphicsShader : public FShader
{
	
};

class FVertexShader : public FGraphicsShader
{
	
};

class FPixelShader : public FGraphicsShader
{
	
};


class UMaterialInterface : public UObject
{
	MY_GENERATED_BODY(UMaterialInterface)

	friend class FShader;

public:
	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

};

class UMaterial : public UMaterialInterface
{
	MY_GENERATED_BODY(UMaterial)
};