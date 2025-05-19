// 04.09
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "Engine/Animation/UAnimCompositeBase.h"
#include "Engine/Animation/UAnimSequence.h"
#include "Engine/Class/Curve/UCurveBase.h"

class UAnimInstance;

struct FCompositeSection
{
	std::string SectionName;
	std::string NextSectionName;
	float       StartTime;
};

struct FAnimTrack
{
	std::vector<std::shared_ptr<UAnimSequence>> AnimSegments;

	float GetLength() const;
};

class UAnimMontage : public UAnimCompositeBase
{
	MY_GENERATE_BODY(UAnimMontage)

	static std::shared_ptr<UAnimMontage> GetAnimationAsset(const std::string& AnimationName)
	{
		if (std::shared_ptr<UAnimationAsset> FindAsset = UAnimationAsset::GetAnimationAsset(AnimationName))
		{
			return std::dynamic_pointer_cast<UAnimMontage>(FindAsset);
		}
		return nullptr;
	}

	FAlphaBlend BlendIn;
	FAlphaBlend BlendOut;

	void LoadDataFromFileData(const nlohmann::json& AssetData) override;

	// 애니메이션 몽타쥬의 전체 길이를 반환
	float GetPlayLength() const
	{
		return AnimTrack.GetLength();
	}

	// 특정 섹션의 시작과 종료 시간을 반환
	float GetStartTimeFromSectionName(const std::string& SectionName);
	void  GetSectionStartAndEndTime(UINT SectionIndex, float& OutStartTime, float& OutEndTime);

	bool IsWithinPos(UINT SectionIndex1, UINT SectionIndex2, float InPosition);
	int  GetSectionIndexFromPosition(float InPosition);
	int  GetSectionIndex(const std::string& InSectionName) const;

public:
	// 언리얼엔진은 FSlotAnimationTrack으로 여러 슬롯으로 관리할 수 있도록 하지만,
	// LeeEngine에서는 하나의 몽타쥬당 하나의 섹션만 관리하도록 진행할 예정
	std::string SlotName;
	FAnimTrack  AnimTrack;

	// 섹션 데이터(Composite Section)
	std::vector<FCompositeSection> CompositeSections;
};

struct FAnimMontageInstance
{
	FAnimMontageInstance(UAnimInstance* InAnimInstance)
		: AnimInstance(InAnimInstance), Position(0.0f)
	{
		MontageBones = std::vector<XMMATRIX>(MAX_BONES, XMMatrixIdentity());
	}

	void Play();

	float GetPosition() const
	{
		return Position;
	}

	void                          SetPosition(float InPosition);
	void                          JumpToSectionName(const std::string& SectionName);
	std::vector<XMMATRIX>         MontageBones;
	std::vector<FAnimNotifyEvent> Notifies;

	std::shared_ptr<UAnimMontage> Montage;

	std::function<void()> OnMontageEnded;
	std::function<void()> OnMontageBlendingOutStarted;
	std::function<void()> OnMontageBlendedInEnded;

	bool  bIsPlaying;
	float CurrentPlayTime = 0.0f;

private:
	UAnimInstance* AnimInstance;
	float          Position;
	float          CurPlayingStartPosition = 0.0f;
	float          CurPlayingEndPosition   = 0.0f;
	std::string    NextSectionName;
};
