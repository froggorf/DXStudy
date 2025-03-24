// 02.16
// ULevel 내 배치를 위한 테스트용 큐브 액터

#pragma once

#include "ATestCube.h"
#include "Engine/GameFramework/AActor.h"

class USkeletalMeshComponent;
class UStaticMeshComponent;

class ATestActor2 : public AActor
{
	MY_GENERATED_BODY(ATestActor2)
public:
	ATestActor2();

	virtual void Tick(float DeltaSeconds) override;
protected:
private:
public:
protected:
private:
	std::shared_ptr<USkeletalMeshComponent> TestSKComp;
};
