// 03.27
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "UAnimSequence.h"
#include "Engine/Components/UAnimMontage.h"
#include "Engine/UObject/UObject.h"

class USkeletalMeshComponent;

class UAnimInstance : public UObject
{
	MY_GENERATED_BODY(UAnimInstance)
public:
	UAnimInstance();
	~UAnimInstance() override {};

	void BeginPlay() override;

	// AnimInstance가 생성될 때 호출되는 함수
	virtual void NativeInitializeAnimation();
	// 매 프레임마다 호출되어 애니메이션 상태머신에 적용될 변수들을 업데이트하는 함수
	virtual void NativeUpdateAnimation(float DeltaSeconds);
	// NativeUpdateAnimation으로 업데이트된 변수를 이용해 애니메이션 상태머신을 통해 애니메이션을 최종으로 판단하는 함수
	virtual void UpdateAnimation(float dt);
	void Tick(float DeltaSeconds);

	void SetSkeletalMeshComponent(USkeletalMeshComponent* InOwner) { CurrentSkeletalMeshComponent = InOwner; }
	USkeletalMeshComponent* GetSkeletalMeshComponent() const { return CurrentSkeletalMeshComponent; }
	class AActor* TryGetPawnOwner() const;

	void Montage_Play(std::shared_ptr<UAnimMontage> MontageToPlay, float InTimeToStartMontageAt = 0.0f);

protected:
	// 애니메이션 레이어 블렌딩
	void LayeredBlendPerBone(const std::vector<XMMATRIX>& BasePose, const std::vector<XMMATRIX>& BlendPose, const std::string& TargetBoneName, float BlendWeights, std::vector<XMMATRIX>& OutMatrices);

	void PlayMontage(const std::string& SlotName, std::vector<XMMATRIX>& OriginMatrices, std::vector<FAnimNotifyEvent>& OriginNotifies);
private:
	
public:
protected:
	float DeltaTime;

	float CurrentTime = 0.0f;
	float LatestUpdateTime = -100.0f;

	// 활성화중인 몽타주와 몽타주 인스턴스 맵
	//std::unordered_map<std::shared_ptr<UAnimMontage>, std::shared_ptr<FAnimMontageInstance>> ActiveMontagesMap;

	// 현재 재생중인 AnimMontage Instances
	std::vector<std::shared_ptr<FAnimMontageInstance>> MontageInstances;
private:

	USkeletalMeshComponent* CurrentSkeletalMeshComponent;
};
