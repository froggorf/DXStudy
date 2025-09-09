// 05.24
// 비동기 에셋 로드를 확인하기 위해 만든 테스트용 큐브액터 2

#pragma once
#include "../Component/PBRTestComponent.h"
#include "../Component/UTestComponent.h"
#include "Engine/Components/USkeletalMeshComponent.h"
#include "Engine/GameFramework/AActor.h"
#include "Engine/Physics/UCapsuleComponent.h"

class UStaticMeshComponent;

class ATestCube2 : public AActor
{
	MY_GENERATE_BODY(ATestCube2)

	ATestCube2();

	void Register() override;
	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;

protected:
	std::shared_ptr<UStaticMeshComponent> PBRTestSphere[10][10];
	std::shared_ptr<UPBRTestComponent> PBRTestComp;

	std::shared_ptr<UStaticMeshComponent> SM_Well;
	std::shared_ptr<UStaticMeshComponent> SM_Barrel1;
	std::shared_ptr<UStaticMeshComponent> SM_Barrel2;
private:


};
