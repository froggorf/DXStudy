#pragma once
#include "Engine/Class/Framework/ACharacter.h"
#include "MyGame/Actor/Decal/ARangeDecalActor.h"
#include "MyGame/Component/Combat/UCombatBaseComponent.h"

/*
	TODO: Note: 캐릭터 작성 중 생각나서 작성
	적군 Base 를 만들 때 OnDeath 를 만들어서 플레이어에게 경험치를 주던 어떠한 작업을 하는 델리게이트를 만들어야함
*/

class UEnemyAnimInstanceBase;
class UWolfAnimInstance;
class AAIController;
class UNiagaraComponent;
class UNiagaraSystem;
class UWidgetComponent;
class UHealthComponent;
class UMotionWarpingComponent;

class AEnemyBase : public ACharacter
{
	MY_GENERATE_BODY(AEnemyBase)

	AEnemyBase();
	~AEnemyBase() override = default;

	void Register() override;
	void Tick(float DeltaSeconds) override;
	void BeginPlay() override;

	float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AActor* DamageCauser) override;

	// Attack Montage를 재생시키고, 몽타쥬 이벤트로 ApplyBasicAttack이 적용
	void PlayAttackMontage(Delegate<> AttackFinishDelegate);
	void ApplyBasicAttack();

	void Death() override;
	void DestroyAtDeath();


	virtual void BindingBehaviorTree();
	virtual std::shared_ptr<UAnimMontage> GetDeathAnimMontage() const {assert(nullptr); return nullptr;}
protected:
	std::string CharacterMeshName = "";
	std::string AnimInstanceName = "UMyGameAnimInstanceBase";
	std::string SkeletalMeshName = "";

	FAttackData BasicAttackData{};
	float EnemyPower = 400.0f;
	bool bBasicAttackApplied = false;

	FTimerHandle DeathTimerHandle;
public:
	const std::shared_ptr<UMotionWarpingComponent>& GetMotionWarpingComponent() const {return MotionWarpingComponent;}

	Delegate<> OnDeath;
protected:
	std::shared_ptr<UMotionWarpingComponent> MotionWarpingComponent;

	std::shared_ptr<UStaticMeshComponent> TempStaticMesh;

	std::shared_ptr<UHealthComponent> HealthComponent;
	std::shared_ptr<UWidgetComponent> HealthWidgetComp;
	

	float EnemyMaxHealth = 500;
	float DetectRange = 700.0f;

	std::shared_ptr<AAIController> AIController;

	std::shared_ptr<UEnemyAnimInstanceBase> EnemyAnimInstance;

};


class AWolf final : public AEnemyBase
{
	MY_GENERATE_BODY(AWolf)

	AWolf();
	~AWolf() override = default;

	void Register() override;

	std::shared_ptr<UAnimMontage> GetDeathAnimMontage() const override {return DeathMontage;}
	std::shared_ptr<UAnimMontage> DeathMontage;
};

class APig final : public AEnemyBase
{
	MY_GENERATE_BODY(APig)

	APig();
	~APig() override = default;

	void Register() override;

	std::shared_ptr<UAnimMontage> GetDeathAnimMontage() const override {return DeathMontage;}
	std::shared_ptr<UAnimMontage> DeathMontage;
};

class AScarecrow final : public AEnemyBase
{
	MY_GENERATE_BODY(AScarecrow)

	AScarecrow();
	~AScarecrow() override = default;

	void Register() override;
	void Tick(float DeltaSeconds) override;
	void BindingBehaviorTree() override;

	std::shared_ptr<UAnimMontage> GetDeathAnimMontage() const override {return DeathMontage;}
	std::shared_ptr<UAnimMontage> DeathMontage;
};

class ADragon final : public AEnemyBase
{
	MY_GENERATE_BODY(ADragon)

	ADragon();
	~ADragon() override = default;

	void Register() override;

	void BindingBehaviorTree() override;

	/// ===================== Skill Range =====================
	void SkillCharging(float Progress);
	void ResetSkillRangeActor();
	/// =======================================================

	/// ==================== Flame Skill ====================
	// 주기적으로 사용하는 스킬의 차지를 시작
	bool StartFlameSkillCharge();
	// 범위 내의 플레이어에게 공격을 가하는 함수
	void StartFlame();
	void EndFlame();

	float GetFlameSkillHalfAngle() const {return FlameHalfAngleDeg; }
	float GetFlameSkillRadius() const {return FlameSkillRadius; }
	/// =====================================================

	/// ==================== HP Skill ====================
	bool StartHPSkillCharge();
	void StartHPSkill();
	void EndHPSkill();

	/// ==================================================
	std::shared_ptr<UAnimMontage> GetDeathAnimMontage() const override {return DeathMontage;}
	std::shared_ptr<UAnimMontage> DeathMontage;
protected:
	std::shared_ptr<UAnimMontage> AM_Dragon_Scream;
	std::shared_ptr<UAnimMontage> AM_Dragon_Flame;

	std::shared_ptr<UAnimMontage> AM_Dragon_FlyStart;
	std::shared_ptr<UAnimMontage> AM_Dragon_Landing;
	std::shared_ptr<UAnimMontage> AM_Dragon_HPFlame;
	float FlameHalfAngleDeg = 60.0f;
	float FlameSkillRadius = 1500.0f;
	float FlameSkillAttackPower = 5.0f;

	XMFLOAT3 HPSkillRange = XMFLOAT3{2000.0f, 10.0f, 3000.0f};

	std::shared_ptr<UNiagaraComponent> FlameVFX;
	std::string FlameSocketName = "Tongue03";
	XMFLOAT3 FlameVFXRotation = XMFLOAT3{90.0f, -0.0f, 0.0f};
	std::shared_ptr<UNiagaraSystem> FlameVFXNormalSystem;
	std::shared_ptr<UNiagaraSystem> FlameVFXHPSkillSystem;
	std::shared_ptr<UNiagaraSystem> ActiveFlameVFXSystem;
	bool bIsFlameVFXReady = false;
	bool bFlameVFXPendingActivate = false;
	bool bIsFlameVFXActive = false;

	// 범위기 표시 장판 액터
	std::shared_ptr<ARangeDecalActor> ShowRangeActor;
private:
	void SetFlameVFXSystem(const std::shared_ptr<UNiagaraSystem>& System);
	void ActivateFlameVFX();
	void DeactivateFlameVFX();
};
