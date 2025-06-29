// 05.24
// 비동기 에셋 로드를 확인하기 위해 만든 테스트용 큐브액터 2

#pragma once
#include "Engine/Components/USkeletalMeshComponent.h"
#include "Engine/GameFramework/AActor.h"
#include "Engine/Physics/UCapsuleComponent.h"


class ATestPawn : public AActor
{
	MY_GENERATE_BODY(ATestPawn)

	ATestPawn();

	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

protected:
	std::shared_ptr<UCapsuleComponent> CapsuleComp;

	std::shared_ptr<USkeletalMeshComponent> SKComp;
private:

};
