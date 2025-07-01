// 05.24
// 비동기 에셋 로드를 확인하기 위해 만든 테스트용 큐브액터 2

#pragma once
#include "../Component/UTestComponent.h"
#include "Engine/Components/USkeletalMeshComponent.h"
#include "Engine/GameFramework/AActor.h"
#include "Engine/Physics/UCapsuleComponent.h"

class UStaticMeshComponent;

class ATestCube2 : public AActor
{
	MY_GENERATE_BODY(ATestCube2)

	ATestCube2();

	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
	void TestCompFunc();
protected:

private:
	std::shared_ptr<UCapsuleComponent> CapsuleComponent;
	std::shared_ptr<UStaticMeshComponent> TestComp;

};
