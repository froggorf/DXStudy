﻿// 04.07
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/FAudioDevice.h"
#include "Engine/UObject/UObject.h"

class UAnimNotify : public UObject
{
	MY_GENERATE_BODY(UAnimNotify)
	UAnimNotify()           = default;
	~UAnimNotify() override = default;

	void         LoadDataFromFileData(const nlohmann::json& AssetData) override;
	virtual const std::string Notify();
};

class UAnimNotify_PlaySound : public UAnimNotify
{
	MY_GENERATE_BODY(UAnimNotify_PlaySound)
	UAnimNotify_PlaySound()           = default;
	~UAnimNotify_PlaySound() override = default;

	void LoadDataFromFileData(const nlohmann::json& AssetData) override;
	const std::string Notify() override;

protected:
	std::shared_ptr<USoundBase> SoundBase;
};


class UAnimNotify_AttackStart : public UAnimNotify
{
	MY_GENERATE_BODY(UAnimNotify_AttackStart)
	UAnimNotify_AttackStart()           = default;
	~UAnimNotify_AttackStart() override = default;

	void LoadDataFromFileData(const nlohmann::json& AssetData) override;
	const std::string Notify() override;
};
class UAnimNotify_AttackEnd : public UAnimNotify
{
	MY_GENERATE_BODY(UAnimNotify_AttackEnd)
	UAnimNotify_AttackEnd()           = default;
	~UAnimNotify_AttackEnd() override = default;

	void LoadDataFromFileData(const nlohmann::json& AssetData) override;
	const std::string Notify() override;
};
