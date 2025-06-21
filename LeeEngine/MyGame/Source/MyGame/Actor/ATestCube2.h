// 05.24
// 비동기 에셋 로드를 확인하기 위해 만든 테스트용 큐브액터 2

#pragma once
#include "../Component/UTestComponent.h"
#include "Engine/Components/USkeletalMeshComponent.h"
#include "Engine/GameFramework/AActor.h"

class UStaticMeshComponent;

class ATestCube2 : public AActor
{
	MY_GENERATE_BODY(ATestCube2)

	ATestCube2();

	void Tick(float DeltaSeconds) override;

protected:
	std::shared_ptr<USkeletalMeshComponent> SK0;
	std::shared_ptr<USkeletalMeshComponent> SK1;
	std::shared_ptr<USkeletalMeshComponent> SK2;
	std::shared_ptr<USkeletalMeshComponent> SK3;
	std::shared_ptr<USkeletalMeshComponent> SK4;
	std::shared_ptr<USkeletalMeshComponent> SK5;
	std::shared_ptr<USkeletalMeshComponent> SK6;
	std::shared_ptr<USkeletalMeshComponent> SK7;
	std::shared_ptr<USkeletalMeshComponent> SK8;
	std::shared_ptr<USkeletalMeshComponent> SK9;
	std::shared_ptr<USkeletalMeshComponent> SK10;
	std::shared_ptr<USkeletalMeshComponent> SK11;
	std::shared_ptr<USkeletalMeshComponent> SK12;
	std::shared_ptr<USkeletalMeshComponent> SK13;
	std::shared_ptr<USkeletalMeshComponent> SK14;

private:
	std::shared_ptr<UTestComponent> TestComp;

};
