// 02.16
// ULevel 내 배치를 위한 테스트용 큐브 액터

#pragma once

#include "Engine/GameFramework/AActor.h"

class UStaticMeshComponent;

class ATestCube : public AActor
{
public:
	ATestCube();
protected:
private:
public:
protected:
private:
	std::shared_ptr<UStaticMeshComponent> TestCubeStaticMeshComp;
	std::shared_ptr<UStaticMeshComponent> TestCube2;
};
