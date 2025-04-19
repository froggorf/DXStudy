#include "CoreMinimal.h"
#include "UTexture.h"

#include "Engine/AssetManager/AssetManager.h"

std::unordered_map<std::string, std::shared_ptr<UTexture>> UTexture::TextureCacheMap;

void UTexture::Release()
{
	if(Texture2D)
	{
		Texture2D->Release();	
	}
	if(SRView)
	{
		SRView->Release();	
	}
	
}

void UTexture::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UObject::LoadDataFromFileData(AssetData);

	if(TextureCacheMap.contains(GetName()))
	{
		// 이미 존재하는 텍스쳐
		assert(0);
		return;
	}

	AssetManager::LoadTexture(this, AssetData);

	TextureCacheMap[GetName()] = shared_from_this();
}
