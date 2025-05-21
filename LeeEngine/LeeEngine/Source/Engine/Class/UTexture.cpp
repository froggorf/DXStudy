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
