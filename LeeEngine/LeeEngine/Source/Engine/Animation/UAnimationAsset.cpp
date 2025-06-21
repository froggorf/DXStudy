#include "CoreMinimal.h"
#include "UAnimationAsset.h"

#include "Engine/AssetManager/AssetManager.h"
#include "Engine/RenderCore/EditorScene.h"

std::shared_ptr<UAnimationAsset> UAnimationAsset::GetAnimationAsset(const std::string& AnimationAssetName)
{
	return std::dynamic_pointer_cast<UAnimationAsset>(AssetManager::GetAssetCacheByName(AnimationAssetName));
}

void UAnimationAsset::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UObject::LoadDataFromFileData(AssetData);

	AssetManager::GetAsyncAssetCache(AssetData["Skeleton"], [this](std::shared_ptr<UObject> Object)
		{
			if (!Object)
			{
				assert(nullptr, "잘못된 스켈레톤 로드");
			}
			AnimationSkeleton = std::dynamic_pointer_cast<USkeletalMesh>(Object);
		});

	// 스켈레톤 정보를 로드받을 떄 까지 busy wait
	while (!AnimationSkeleton)
	{
		std::this_thread::yield();
	}

	int a = 0;
}
