// 03.27
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/MyEngineUtils.h"
#include "Engine/UObject/UObject.h"
#include "Engine/Mesh/USkeletalMesh.h"

class UAnimationAsset : public UObject, public std::enable_shared_from_this<UAnimationAsset>
{
	MY_GENERATE_BODY(UAnimationAsset)

	const std::shared_ptr<USkeletalMesh>& GetAnimationSkeleton() const
	{
		return AnimationSkeleton;
	}

	static std::shared_ptr<UAnimationAsset> GetAnimationAsset(const std::string& AnimationAssetName)
	{
		const auto& Map = GetAnimationAssetCacheMap();
		if (Map.size() == 0)
			return nullptr;
		if (Map.contains(AnimationAssetName))
		{
			return GetAnimationAssetCacheMap()[AnimationAssetName];
		}
		return nullptr;
	}

	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

protected:
	std::shared_ptr<USkeletalMesh> AnimationSkeleton;

private:
	static std::map<std::string, std::shared_ptr<UAnimationAsset>>& GetAnimationAssetCacheMap()
	{
		static std::map<std::string, std::shared_ptr<UAnimationAsset>> AnimationAssetCache{};
		return AnimationAssetCache;
	}
};
