// 03.29
// 애니메이션을 위한 임시 액터 컴퍼넌트

#pragma once
#include "Engine/Components/UActorComponent.h"
#include "Engine/Components/UAnimMontage.h"
#include "Engine/Timer/FTimerManager.h"

class AMyGameCharacterBase;
class UPlayerInput;
class AMyGameCharacterBase;

struct FAttackData
{
	FAttackData() = default;
	FAttackData(const XMFLOAT3& Range, float Damage, float MoveDistance, float GainUltimateGauge, bool bAttackCenterFixed = false, const XMFLOAT3& AtkCenterPos ={})
	{
		this->AttackRange = Range;
		this->DamagePercent = Damage;
		this->MoveDistance = MoveDistance;
		this->bIsAttackCenterFixed = bAttackCenterFixed;
		this->AttackCenterPos = AtkCenterPos;
		this->GainUltimateGauge = GainUltimateGauge;
	}
	std::shared_ptr<UAnimMontage> AnimMontage;
	XMFLOAT3 AttackRange;
	float DamagePercent = 1.0f;
	float MoveDistance = 0.0f;
	float GainUltimateGauge = 10.0f;

	bool bIsAttackCenterFixed = false;
	XMFLOAT3 AttackCenterPos;
};

class UCombatBaseComponent : public UActorComponent
{
	MY_GENERATE_BODY(UCombatBaseComponent)
	UCombatBaseComponent();

public:
	void BeginPlay() override;

	// AMyGameCharacterBase 내에서 Register 에 호출됨
	// 초기 세팅
	virtual void Initialize(AMyGameCharacterBase* MyCharacter);

	virtual void BasicAttack();
	virtual void BasicAttackEnded();
	virtual void HeavyAttack();
	virtual void HeavyAttackMouseReleased() {}
	virtual void JumpAttack() {};
	virtual void BindKeyInputs(const std::shared_ptr<UPlayerInput>& InputSystem);
protected:
	// Initialize 안에서 적용
	void SetBasicAttackData(const std::vector<std::string>& AttackMontageNames, const std::vector<FAttackData>& NewAttackData);
	void SetHeavyAttackData(const std::vector<std::string>& AttackMontageNames, const std::vector<FAttackData>& NewAttackData);
protected:
	AMyGameCharacterBase* MyGameCharacter = nullptr;

public:
	void SetFightMode(bool NewMode);
	bool FindNearbyEnemy(const XMFLOAT3& SpherePos , float EnemyFindRadius , const std::vector<AActor*>& IgnoreActors, std::vector<AActor*>& OverlapActors);
	AActor* FindNearestEnemy(const XMFLOAT3& SpherePos , float EnemyFindRadius , const std::vector<AActor*>& IgnoreActors);

	const FAttackData& GetBasicAttackData(size_t Index);
	const FAttackData& GetHeavyAttackData(size_t Index);

protected:
	// 초기 세팅을 위해
	bool bIsFightMode = true;

	// ===================== Basic Attack =====================
	// BasicAttackData.size() 가 기본공격의 콤보 수
	// BasicAttackData, 재생할 몽타쥬, 공격범위, 퍼뎀 등의 정보가 담겨있음
	std::vector<FAttackData> BasicAttackData;
	std::vector<FAttackData> HeavyAttackData;

	// 선입력을 위해 키입력 시간을 저장
	float LastBasicAttackClickedTime = -1.0f;
	// 선입력 시간 : 0.5f
	float BasicAttackApplyTime = 0.5f;
	// 현재의 기본공격 단계를 저장
	UINT CurrentBasicAttackCombo = 0;
	bool bIsBasicAttacking = false;


	// ===================== HeavyAttack =====================
	float LastLeftMouseClickedTime = -1.0f;
	bool bIsHeavyAttacking = false;


};
