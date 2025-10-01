#pragma once
#include "Engine/Animation/AnimNotifies/AnimNotify.h"

class UAnimNotify_MotionWarpingStart : public UAnimNotify
{
	MY_GENERATE_BODY(UAnimNotify_MotionWarpingStart)

	const std::string& Notify() override;

	void LoadDataFromFileData(const nlohmann::json& AssetData) override;
	std::string WarpingName = "MotionWarping";
};



class UAnimNotify_MotionWarpingEnd : public UAnimNotify
{
	MY_GENERATE_BODY(UAnimNotify_MotionWarpingEnd)

	const std::string& Notify() override;

};
