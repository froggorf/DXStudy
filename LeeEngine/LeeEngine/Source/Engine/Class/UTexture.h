// 04.19
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/MyEngineUtils.h"
#include "Engine/UObject/UObject.h"

class UTexture : public UObject, public std::enable_shared_from_this<UTexture>
{
	MY_GENERATED_BODY(UTexture)

	friend class AssetManager;
public:
	UTexture() = default;

	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

protected:
private:
public:
protected:
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
	// SRV / RTV / DSV / UAV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRView;
	//...
	D3D11_TEXTURE2D_DESC Desc{};

	std::unordered_map<std::string, std::shared_ptr<UTexture>> TextureCacheMap;
};