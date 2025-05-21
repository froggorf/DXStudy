// 04.19
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/UObject/UObject.h"

class UTexture : public UObject
{
	MY_GENERATE_BODY(UTexture)

	friend class AssetManager;
	UTexture() = default;
	void Release();
	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

	UINT GetWidth() const
	{
		return Desc.Width;
	}

	UINT GetHeight() const
	{
		return Desc.Height;
	}

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSRV() const
	{
		return SRView;
	}

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> GetRTV() const
	{
		return RTView;
	}

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> GetDSV() const
	{
		return DSView;
	}

	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> GetUAV() const
	{
		return UAView;
	}

	static std::shared_ptr<UTexture> GetTextureCache(const std::string& TextureName);

private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> Texture2D;
	// SRV / RTV / DSV / UAV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>  SRView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>    RTView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>    DSView;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> UAView;
	//...
	D3D11_TEXTURE2D_DESC Desc{};
};
