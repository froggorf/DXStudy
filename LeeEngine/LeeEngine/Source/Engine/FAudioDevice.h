// 04.08
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "CoreMinimal.h"
#include "Engine/MyEngineUtils.h"
#include "UObject/UObject.h"

class USoundBase : public UObject
{
	MY_GENERATED_BODY(USoundBase)
public:
	std::string SoundName;
protected:
private:
public:
protected:
private:
};

class FActiveSound
{
private:
	std::shared_ptr<USoundBase> Sound;
	UINT AudioComponentID = -1;
	
};

class FAudioDevice 
{
	std::vector<FActiveSound> ActiveSounds;
	std::queue<std::shared_ptr<FActiveSound>> PendingAddedActiveSounds;
};