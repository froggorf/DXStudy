#include "CoreMinimal.h"
#include "UTexture.h"

#include "Engine/AssetManager/AssetManager.h"

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
