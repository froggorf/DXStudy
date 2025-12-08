// 03.27
// 언리얼 엔진 5 코드를 분석하며 자체엔진으로 작성중인 코드입니다.
// 언리얼엔진의 코딩컨벤션을 따릅니다.  https://dev.epicgames.com/documentation/ko-kr/unreal-engine/coding-standard?application_version=4.27
// 이윤석

#pragma once
#include "UAnimSequence.h"
#include "Engine/Components/UAnimMontage.h"
#include "Engine/Misc/Delegate.h"
#include "Engine/UObject/UObject.h"

enum class EAnimState
{
	Locomotion,
	Jump,
	Falling,
	Landing
};


class USkeletalMeshComponent;


class UAnimInstance : public UObject
{
	MY_GENERATE_BODY(UAnimInstance)
	UAnimInstance();

	~UAnimInstance() override = default;

	virtual void SetAnimNotify_BeginPlay() {}
	void BeginPlay() override;

	// AnimInstance가 생성될 때 호출되는 함수
	virtual void NativeInitializeAnimation();
	// 매 프레임마다 호출되어 애니메이션 상태머신에 적용될 변수들을 업데이트하는 함수
	virtual void NativeUpdateAnimation(float DeltaSeconds);
	// NativeUpdateAnimation으로 업데이트된 변수를 이용해 애니메이션 상태머신을 통해 애니메이션을 최종으로 판단하는 함수
	virtual void UpdateAnimation(float dt);

	// FBoneLocalTransform 에서 BoneMatrices(스키닝 행렬) 을 구하는 함수
	void CalculateFinalBoneMatrices(std::vector<XMMATRIX>& FinalBoneMatrices);
	void CalculateBoneMatrices(const std::vector<FBoneLocalTransform>& BoneTransforms, std::vector<XMMATRIX>& BoneMatrices);
	// 스키닝 행렬에서 FBoneLocalTransform 을 구하는 함수
	void DecomposeBoneMatricesToBoneLocalTransForms(const std::vector<XMMATRIX>& BoneMatrices, std::vector<FBoneLocalTransform>& BoneTransforms);
	void         Tick(float DeltaSeconds);

	void SetSkeletalMeshComponent(USkeletalMeshComponent* InOwner)
	{
		CurrentSkeletalMeshComponent = InOwner;
	}

	USkeletalMeshComponent* GetSkeletalMeshComponent() const
	{
		return CurrentSkeletalMeshComponent;
	}

	class AActor* TryGetPawnOwner() const;

	void Montage_Play(std::shared_ptr<UAnimMontage> MontageToPlay, float InTimeToStartMontageAt = 0.0f, const Delegate<>& OnMontageEnd = Delegate<>(), const Delegate<>& OnMontageBlendingInStart = Delegate<>(), const Delegate<>& OnMontageBlendOutStart = Delegate<>());
	const std::vector<XMMATRIX>& GetGlobalBoneTransforms() const {return GlobalTransforms;}

	std::vector<FBoneLocalTransform> GetInitialLocalBoneTransforms() const;
	bool IsPlayingMontage() const {return bPlayRootMotion;}

	void ApplyAdditiveAnimation(const std::vector<FBoneLocalTransform>& BasePose, const std::vector<FBoneLocalTransform>& AdditivePose, float Weight, std::vector<FBoneLocalTransform>& OutPose);

	void JumpStart();
	void FallingStart()
	{
		AnimState = EAnimState::Falling;
		CurrentFallingTime = 0.0f;
	}
	void LandingStart()
	{
		AnimState = EAnimState::Landing;
		CurrentLandingTime = 0.0f;
	}
protected:
	EAnimState AnimState = EAnimState::Locomotion;
	float CurrentJumpStartTime = 0.0f;
	float CurrentFallingTime = 0.0f;
	float CurrentLandingTime = 0.0f;


protected:
	// 현재 애님 인스턴스의 리소스가 모두 정상적으로 존재하는지 체크하는 함수
	virtual bool IsAllResourceOK();

	// 애니메이션 레이어 블렌딩
	void LayeredBlendPerBone(const std::vector<FBoneLocalTransform>& BasePose, const std::vector<FBoneLocalTransform>& BlendPose, const std::string& TargetBoneName, float BlendWeights, std::vector<FBoneLocalTransform>& OutBoneTransforms);

	bool PlayMontage(const std::string& SlotName, std::vector<FBoneLocalTransform>& OriginBoneTransforms, std::vector<FAnimNotifyEvent>& OriginNotifies);

	float DeltaTime;

	float CurrentTime      = 0.0f;
	float LatestUpdateTime = -100.0f;

	// 활성화중인 몽타주와 몽타주 인스턴스 맵
	//std::unordered_map<std::shared_ptr<UAnimMontage>, std::shared_ptr<FAnimMontageInstance>> ActiveMontagesMap;

	// 현재 재생중인 AnimMontage Instances
	std::vector<std::shared_ptr<FAnimMontageInstance>> MontageInstances;

protected:
	// 스켈레탈 메시의 SocketTransform 계산시 사용
	std::vector<XMMATRIX> GlobalTransforms;

	// Notify Map
	// {노티파이 이름, 델리게이트}
	std::map<std::string, Delegate<>> NotifyEvent;

	std::vector<FBoneLocalTransform> BoneTransforms;
	std::vector<FAnimNotifyEvent> FinalNotifies;

	bool bPlayRootMotion = false;
	bool bBlendOut = false;
	bool bUseMotionWarping = false;

public:
	static constexpr float AnimTickFPS = 30;
protected:
	class ACharacter* OwnerCharacter = nullptr;
private:
	USkeletalMeshComponent* CurrentSkeletalMeshComponent;
	bool bIsAnimationLoaded = false;


};
