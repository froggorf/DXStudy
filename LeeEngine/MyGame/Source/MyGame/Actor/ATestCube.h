// 02.16
// ULevel 내 배치를 위한 테스트용 큐브 액터

#pragma once

#include "Engine/GameFramework/AActor.h"

class UStaticMeshComponent;

class ATestCube : public AActor
{
	MY_GENERATED_BODY(ATestCube)
public:
	ATestCube();

	virtual void Tick(float DeltaSeconds) override;
protected:
private:
public:
protected:
	std::shared_ptr<UStaticMeshComponent> TestCubeStaticMeshComp;
private:
	std::shared_ptr<UStaticMeshComponent> TestCube2;
	std::shared_ptr<UStaticMeshComponent> TestCube3;
	std::shared_ptr<UStaticMeshComponent> TestCube4;
};
