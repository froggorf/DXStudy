#pragma once
#include "MyGame/AnimInstance/UMyGameAniminstanceBase.h"

class UGideonAnimInstance final : public UMyGameAnimInstanceBase
{
	MY_GENERATE_BODY(UGideonAnimInstance)

	UGideonAnimInstance();

	void LoadData_OnRegister() override;
	void SetAnimNotify_BeginPlay() override;

	bool IsAllResourceOK() override;

	void NativeUpdateAnimation(float DeltaSeconds) override;
	void UpdateAnimation(float dt) override;

	void Ultimate_StartMotionWarping();
	void Ultimate_ChangeCameraToNormal();
	// 애님몽타쥬의 PlayRate -1 작업을 위해 분리하여 작업
	void Ultimate_ChangeToLoop();
	void Ultimate_Loop2();
	void Ultimate_End();

	void               BasicAttack0();
	void               BasicAttack1();
	void               BasicAttack2();
	void               HeavyAttack();
	void               SkillAttack();

	std::string GetJumpStartAnimSequenceName() override { return "AS_Gideon_JumpStart";};
	std::string GetFallingAnimSequenceName() override { return "AS_Gideon_Falling";};
	std::string GetLandAnimSequenceName() override { return "AS_Gideon_Land";};
	float GetJumpStartTimeMultiply() override {return 1.5f;}
	float GetLandingTimeMultiply() override {return 2.0f;}

private:
	std::shared_ptr<UBlendSpace> BS_FPSLocomotion;
	std::shared_ptr<UAnimMontage> AM_UltLoop1;
	std::shared_ptr<UAnimMontage> AM_UltLoop2;
	std::shared_ptr<UAnimMontage> AM_UltEnd;
	float Direction = 0.0f;
	

	// 비동기 로드 에셋 캐싱
	static std::shared_ptr<UAnimMontage> Cache_AM_Ult_Loop1;
	static std::shared_ptr<UAnimMontage> Cache_AM_Ult_Loop2;
	static std::shared_ptr<UAnimMontage> Cache_AM_Ult_End;
	static std::shared_ptr<UBlendSpace> Cache_BS_Gideon_FPS_Loco;
};
