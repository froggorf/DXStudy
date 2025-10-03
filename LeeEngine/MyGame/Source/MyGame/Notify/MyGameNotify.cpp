#include "CoreMinimal.h"
#include "MyGameNotify.h"

const std::string UAnimNotify_MotionWarpingStart::Notify()
{
	return WarpingName;
}

void UAnimNotify_MotionWarpingStart::LoadDataFromFileData(const nlohmann::json& AssetData)
{
	UAnimNotify::LoadDataFromFileData(AssetData);

	WarpingName = AssetData["WarpingName"];
}

const std::string UAnimNotify_MotionWarpingEnd::Notify()
{
	return "MotionWarpingEnd";
}
