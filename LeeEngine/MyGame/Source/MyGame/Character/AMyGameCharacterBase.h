// 05.24
// 비동기 에셋 로드를 확인하기 위해 만든 테스트용 큐브액터 2

#pragma once
#include "Engine/Class/Framework/ACharacter.h"
#include "MyGame/Component/Combat/UCombatBaseComponent.h"
#include "MyGame/Component/Combat/Skill/NormalSkill/USkillBaseComponent.h"
#include "MyGame/Component/Combat/Skill/Ultimate/UUltimateBaseComponent.h"
#include "MyGame/Component/MotionWarping/UMotionWarpingComponent.h"
#include "MyGame/Interface/Dodge/IDodgeInterface.h"

class UMyGameWidgetBase;

class AMyGameCharacterBase : public ACharacter, public IDodgeInterface
{
	MY_GENERATE_BODY(AMyGameCharacterBase)
	
	AMyGameCharacterBase();
	~AMyGameCharacterBase() override = default;

	void Register() override;
	virtual void LoadCharacterData_OnRegister();
	void BeginPlay() override;

	void BindKeyInputs() override;

	float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AActor* DamageCauser) override;

	void Tick(float DeltaSeconds) override;
	void Move(float X, float Y);
	void Look(float X, float Y);
	void MouseRotateStart();
	void MouseRotateEnd();
	void SetWalk();
	void SetRun();
	void WheelUp();
	void WheelDown();

	virtual void CreateWidgetOnBeginPlay() {}

	const std::shared_ptr<UCombatBaseComponent>& GetCombatComponent() const {return CombatComponent;}
	const std::shared_ptr<USkillBaseComponent>& GetSkillComponent() const {return SkillComponent;}
	const std::shared_ptr<UUltimateBaseComponent>& GetUltimateComponent() const {return UltimateComponent;}
	std::shared_ptr<UMyGameWidgetBase> GetCharacterWidget() const {return CharacterWidget.lock(); }

private:
	bool bRightButtonPressed = false;

	// IDodgeInterface
protected:
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
protected:
	std::string CharacterMeshName = "SK_Manny_UE4";
	std::string AnimInstanceName = "UMyGameAnimInstanceBase";

	// 캐릭터 마다 존재하는 위젯 (BeginPlay에서 생성)
	std::weak_ptr<UMyGameWidgetBase> CharacterWidget;
public:
	const std::shared_ptr<UMotionWarpingComponent>& GetMotionWarpingComponent() const {return MotionWarpingComponent;}
protected:
	std::shared_ptr<UMotionWarpingComponent> MotionWarpingComponent;
};
