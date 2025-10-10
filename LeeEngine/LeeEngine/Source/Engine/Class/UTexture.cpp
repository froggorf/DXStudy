#include "CoreMinimal.h"
#include "UTexture.h"

#include "Engine/AssetManager/AssetManager.h"

void UTexture::Release()
{
	if (Texture2D)
	{
		Texture2D->Release();
	}
	if (SRView)
	{
		SRView->Release();
	}
}

void UTexture::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UObject::LoadDataFromFileData(AssetData);

	AssetManager::LoadTexture(this, AssetData);
}

std::shared_ptr<UTexture> UTexture::GetTextureCache(const std::string& TextureName)
{
	return std::dynamic_pointer_cast<UTexture>(AssetManager::GetAssetCacheByName(TextureName));
}

int UTexture::Create(Microsoft::WRL::ComPtr<ID3D11Texture2D> Tex2D)
{
	Texture2D = Tex2D;
	Texture2D->GetDesc(&Desc);

	// View 제작
	if (Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		if (FAILED(GDirectXDevice->GetDevice()->CreateRenderTargetView(Texture2D.Get(), nullptr, RTView.GetAddressOf())))
			return E_FAIL;
	}

	if (Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		if (FAILED(GDirectXDevice->GetDevice()->CreateDepthStencilView(Texture2D.Get(), nullptr, DSView.GetAddressOf())))
			return E_FAIL;
	}

	if (Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		if (FAILED(GDirectXDevice->GetDevice()->CreateShaderResourceView(Texture2D.Get(), nullptr, SRView.GetAddressOf())))
			return E_FAIL;
	}

	if (Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		if (FAILED(GDirectXDevice->GetDevice()->CreateUnorderedAccessView(Texture2D.Get(), nullptr, UAView.GetAddressOf())))
			return E_FAIL;
	}

	return S_OK;
}
