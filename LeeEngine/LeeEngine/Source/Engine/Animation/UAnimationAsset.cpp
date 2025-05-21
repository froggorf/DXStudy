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
	std::string Test     = AssetData["Name"];
	UObject::LoadDataFromFileData(AssetData);

	std::shared_ptr<USkeletalMesh> Skeleton = USkeletalMesh::GetSkeletalMesh(AssetData["Skeleton"]);
	if (!Skeleton)
	{
		assert(nullptr);
		return;
	}
	AnimationSkeleton           = Skeleton;
}
