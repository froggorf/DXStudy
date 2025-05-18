// 02.16
// ULevel 내 배치를 위한 테스트용 큐브 액터

#pragma once

#include "ATestCube.h"
#include "../Component/UTestComponent.h"
#include "Engine/GameFramework/AActor.h"

class USkeletalMeshComponent;
class UStaticMeshComponent;

class ATestActor2 : public AActor
{
	MY_GENERATED_BODY(ATestActor2)
public:
	ATestActor2();
	void Tick(float DeltaSeconds) override;

private:
	std::shared_ptr<USkeletalMeshComponent> TestSKComp;
	std::shared_ptr<UTestComponent> TestComponent;
	std::shared_ptr<UNiagaraComponent> NiagaraComp;
};
