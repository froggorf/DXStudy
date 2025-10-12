#pragma once
#include "Engine/Class/Framework/ACharacter.h"

/*
	TODO: Note: 캐릭터 작성 중 생각나서 작성
	적군 Base 를 만들 때 OnDeath 를 만들어서 플레이어에게 경험치를 주던 어떠한 작업을 하는 델리게이트를 만들어야함
*/

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

	void Death() override;

	void Tick(float DeltaSeconds) override;


protected:
	std::string CharacterMeshName = "SK_Manny_UE4";
	std::string AnimInstanceName = "UMyGameAnimInstanceBase";

public:
	const std::shared_ptr<UMotionWarpingComponent>& GetMotionWarpingComponent() const {return MotionWarpingComponent;}
	
protected:
	std::shared_ptr<UMotionWarpingComponent> MotionWarpingComponent;

	std::shared_ptr<UStaticMeshComponent> TempStaticMesh;

	std::shared_ptr<UHealthComponent> HealthComponent;
	std::shared_ptr<UWidgetComponent> HealthWidgetComp;
	

	float EnemyMaxHealth = 10000;

};
