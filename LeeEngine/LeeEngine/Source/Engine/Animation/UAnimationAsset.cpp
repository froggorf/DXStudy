#include "CoreMinimal.h"
#include "UAnimationAsset.h"

#include "Engine/RenderCore/EditorScene.h"

void UAnimationAsset::LoadDataFromFileData(const nlohmann::json& AssetData)
{
    auto& CacheMap = GetAnimationAssetCacheMap();
    std::string Test = AssetData["Name"] ;
    std::cout<< Test << std::endl;
    if(CacheMap.contains(AssetData["Name"]))
    {
        MY_LOG("UAnimationAsset", EDebugLogLevel::DLL_Warning, "Already loaded AnimationAsset - " + AssetData["Name"]);
        return;
    }

    UObject::LoadDataFromFileData(AssetData);

    std::shared_ptr<USkeletalMesh> Skeleton = USkeletalMesh::GetSkeletalMesh(AssetData["Skeleton"]);
    if(!Skeleton)
    {
        return;
    }
    AnimationSkeleton = Skeleton;
    CacheMap[AssetData["Name"]] = shared_from_this();
}
