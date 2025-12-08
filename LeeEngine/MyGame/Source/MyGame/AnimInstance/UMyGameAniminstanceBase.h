#pragma once
#include "Engine/Animation/UBlendSpace.h"
#include "Engine/Animation/UAnimInstance.h"
#include "Engine/Components/UAnimMontage.h"
#include "Engine/Misc/Delegate.h"

class AMyGameCharacterBase;
class UCharacterMovementComponent;


class UMyGameAnimInstanceBase : public UAnimInstance
{
	MY_GENERATE_BODY(UMyGameAnimInstanceBase)

	UMyGameAnimInstanceBase();
	~UMyGameAnimInstanceBase() override = default;

	// 기존에는 abstract 로 만들었으나
	// 다른 하위 클래스를 모두 수정해줄 필요는 없기에 다음과 같이 조정
	// 필요한 캐릭터만 오버라이딩하여 적용
	virtual std::string GetJumpStartAnimSequenceName() {return "";};
	virtual std::string GetFallingAnimSequenceName() {return "";};
	virtual std::string GetLandAnimSequenceName() {return "";};

	virtual void LoadData_OnRegister();
	void         Register() override;

	void     SetAnimNotify_BeginPlay() override;
	void     BeginPlay() override;
	void     NativeInitializeAnimation() override;
	void     NativeUpdateAnimation(float DeltaSeconds) override;
	void     UpdateAnimation(float dt) override;

	// BasicAttackComboIndex 번째 기본공격 몽타쥬를 기반으로 적용되며 (거리이동, 시간 등)
	// 적이 없을 경우엔 IfNoEnemyWarpingDirectionUnitVector 방향으로 이동
	void SetWarpingTarget(const XMFLOAT3& IfNoEnemyWarpingDirectionUnitVector, float MoveDistance, float MoveTime);
	XMFLOAT3 GetWarpingPositionToEnemy(const XMFLOAT3& CurActorLocation , XMFLOAT3& EnemyLocation , float MaxMoveDistance);
	void EndMotionWarping();
	bool SetMotionWarping();

	
	bool IsAllResourceOK() override;

protected:
	std::string BS_LocomotionName = "BS_UE4_Locomotion";

protected:

	AMyGameCharacterBase* MyGameCharacter = nullptr;

	float MovementVelocity = 0.0f;
	UCharacterMovementComponent* MovementComp;
	std::shared_ptr<UBlendSpace> BS_Locomotion;

	std::shared_ptr<UAnimSequence> AS_JumpStart;
	std::shared_ptr<UAnimSequence> AS_Falling;
	std::shared_ptr<UAnimSequence> AS_Land;

	

private:
};
