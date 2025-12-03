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
class UWidgetComponent;
class UHealthComponent;
class UMotionWarpingComponent;

class AEnemyBase : public ACharacter
{
	MY_GENERATE_BODY(AEnemyBase)

	AEnemyBase();
	~AEnemyBase() override = default;

	void Register() override;
	void BeginPlay() override;

	float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AActor* DamageCauser) override;

	// Attack Montage를 재생시키고, 몽타쥬 이벤트로 ApplyBasicAttack이 적용
	void PlayAttackMontage(Delegate<> AttackFinishDelegate);
	void ApplyBasicAttack();

	void Death() override;

	void Tick(float DeltaSeconds) override;

	virtual void BindingBehaviorTree();


protected:
	std::string CharacterMeshName = "";
	std::string AnimInstanceName = "UMyGameAnimInstanceBase";
	std::string SkeletalMeshName = "";

	FAttackData BasicAttackData{};
	float EnemyPower = 50.0f;

public:
	const std::shared_ptr<UMotionWarpingComponent>& GetMotionWarpingComponent() const {return MotionWarpingComponent;}
	
protected:
	std::shared_ptr<UMotionWarpingComponent> MotionWarpingComponent;

	std::shared_ptr<UStaticMeshComponent> TempStaticMesh;

	std::shared_ptr<UHealthComponent> HealthComponent;
	std::shared_ptr<UWidgetComponent> HealthWidgetComp;
	

	float EnemyMaxHealth = 500;

	std::shared_ptr<AAIController> AIController;

	std::shared_ptr<UEnemyAnimInstanceBase> EnemyAnimInstance;
};


class AWolf final : public AEnemyBase
{
	MY_GENERATE_BODY(AWolf)

	AWolf();
	~AWolf() override = default;
};

class APig final : public AEnemyBase
{
	MY_GENERATE_BODY(APig)

	APig();
	~APig() override = default;
};

class ADragon final : public AEnemyBase
{
	MY_GENERATE_BODY(ADragon)

	ADragon();
	~ADragon() override = default;

	void Register() override;

	void BindingBehaviorTree() override;

	/// ==================== Flame Skill ====================
	// 주기적으로 사용하는 스킬의 차지를 시작
	bool StartFlameSkillCharge();

	// 범위 내의 플레이어에게 공격을 가하는 함수
	void StartFlame();
	void EndFlame();

	float GetFlameSkillHalfAngle() const {return FlameHalfAngleDeg; }
	float GetFlameSkillRadius() const {return FlameSkillRadius; }
	/// =====================================================
	
protected:
	std::shared_ptr<UAnimMontage> AM_Dragon_Scream;
	std::shared_ptr<UAnimMontage> AM_Dragon_Flame;
	std::shared_ptr<UAnimMontage> AM_Dragon_FlyStart;
	std::shared_ptr<UAnimMontage> AM_Dragon_Landing;
	float FlameHalfAngleDeg = 60.0f;
	float FlameSkillRadius = 1500.0f;
	float FlameSkillAttackPower = 5.0f;
private:
};