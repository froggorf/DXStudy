// 05.24
// 비동기 에셋 로드를 확인하기 위해 만든 테스트용 큐브액터 2

#pragma once
#include "../Component/UTestComponent.h"
#include "Engine/Class/Framework/ACharacter.h"


class ATestPawn : public ACharacter
{
	MY_GENERATE_BODY(ATestPawn)
	
	ATestPawn();
	
	void BeginPlay() override;

	void BindKeyInputs() override;

	void Tick(float DeltaSeconds) override;
	void OnComponentHitEvent(UShapeComponent* HitComponent, AActor* OtherActor, UShapeComponent* OtherComp, const FHitResult& HitResults);
	void AttackStart(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp);
	void AttackEnd(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp);

	void SetAttackStart();
	void SetAttackEnd();

	void Attack();
	void Backstep();
	void Move(float X, float Y);
	void Look(float X, float Y);
	void MouseRotateStart();
	void MouseRotateEnd();
	void SetWalk();
	void SetRun();

protected:
	std::weak_ptr<UTestComponent> TestComp;
	std::shared_ptr<UStaticMeshComponent> SMSword;


	std::shared_ptr<UAnimMontage> AM_Sword;
	std::shared_ptr<UAnimMontage> AM_Smash;

private:
	bool bRightButtonPressed = false;
};
