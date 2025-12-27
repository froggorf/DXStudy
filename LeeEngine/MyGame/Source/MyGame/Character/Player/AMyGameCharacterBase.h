// 05.24
// 비동기 에셋 로드를 확인하기 위해 만든 테스트용 큐브액터 2

#pragma once
#include "Engine/Class/Framework/ACharacter.h"
#include "MyGame/Component/Combat/UCombatBaseComponent.h"
#include "MyGame/Component/MotionWarping/UMotionWarpingComponent.h"
#include "MyGame/Interface/Dodge/IDodgeInterface.h"

class UHealthComponent;
class UMyGameWidgetBase;
class USkillBaseComponent;
class UUltimateBaseComponent;

enum class EElementType
{
	Fusion,		// 융용
	Glacio,		// 응결
	Conducto,	// 전도
	Spectra,	// 회절
	Aero,		// 기류
	Havoc		// 인멸
};

enum class EEquipType
{
	Head,
	Armor,
	Glove,
	Weapon,
	Count
};

XMFLOAT4 GetElementColor(EElementType ElementType);
struct FMyGameDamageEvent : public FDamageEvent
{
	~FMyGameDamageEvent() override = default;
	EElementType ElementType;
};

class AMyGameCharacterBase : public ACharacter, public IDodgeInterface
{
	MY_GENERATE_BODY(AMyGameCharacterBase)

	AMyGameCharacterBase();
	~AMyGameCharacterBase() override = default;

	void Register() override;
	virtual void LoadCharacterData_OnRegister();
	void BeginPlay() override;
	void Tick_Editor(float DeltaSeconds) override;

	void BindKeyInputs() override;

	virtual void OnSkeletalMeshLoadSuccess();

	// 공격에 성공할 시 true 반환
	bool ApplyDamageToEnemy_Range(const FAttackData& AttackData, const std::string& DamageType = "");
	void ApplyDamageToEnemy(AActor* DamagedEnemy, const FAttackData& AttackData, const std::string& DamageType = "");
	float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AActor* DamageCauser) override;

	void Death() override;

	void Tick(float DeltaSeconds) override;
	void Move(float X, float Y);
	virtual void Look(float X, float Y);
	void AltButtonPressed();
	void AltButtonReleased();
	void SetWalk();
	void SetRun();
	void WheelUp();
	void WheelDown();

	void TryUsePotion();
	void ApplyPotion();

	void Debug_SetUltimateGauge100();

	float GetCurrentPower() const;
	float GetWeaponPower() const;
	float GetArmorPower() const;
	void SetEquipmentLevel(const std::array<int, static_cast<int>(EEquipType::Count)>& NewEquipArr);
protected:
	float MaxPower = 120.0f;
	float MinPower = 80.0f;
	std::array<float, static_cast<int>(EEquipType::Count)> EquipLevel = {0,0,0,0};
public:
	const std::shared_ptr<UCombatBaseComponent>& GetCombatComponent() const {return CombatComponent;}
	const std::shared_ptr<USkillBaseComponent>& GetSkillComponent() const {return SkillComponent;}
	const std::shared_ptr<UUltimateBaseComponent>& GetUltimateComponent() const {return UltimateComponent;}
	std::shared_ptr<UMyGameWidgetBase> GetCharacterWidget() const {return CharacterWidget; }

private:
	bool bAltButtonPressed = false;

protected:
	// IDodgeInterface
	void Dodge() override;
	void DodgeEnd() override;
	void ChangeToRoll() override;
	void RollEnd() override;
	void AttackedWhileDodge() override;

protected:
	// ================= 전투 관련 컴퍼넌트 =================
	std::shared_ptr<UCombatBaseComponent> CombatComponent;
	std::shared_ptr<USkillBaseComponent> SkillComponent;
	std::shared_ptr<UUltimateBaseComponent> UltimateComponent;

	std::shared_ptr<UHealthComponent> HealthComponent;

	// 궁극기 시전 시 전환될 카메라
	std::shared_ptr<UCameraComponent> UltimateSceneCameraComp;
	std::shared_ptr<USceneComponent> TestComp_CheckCameraPos;
	std::shared_ptr<UStaticMeshComponent> SM_Arrow;

public:
	void ChangeToUltimateCamera();
	void ChangeToNormalCamera(float BlendTime);
protected:
	std::string CharacterMeshName = "SK_Manny_UE4";
	std::string AnimInstanceName = "UMyGameAnimInstanceBase";

	std::shared_ptr<UMyGameWidgetBase> CharacterWidget;
public:
	const std::shared_ptr<UMotionWarpingComponent>& GetMotionWarpingComponent() const {return MotionWarpingComponent;}
protected:
	std::shared_ptr<UMotionWarpingComponent> MotionWarpingComponent;

	float CharacterMaxHealth = 10000.0f;

	EElementType ElementType = EElementType::Aero;

};
