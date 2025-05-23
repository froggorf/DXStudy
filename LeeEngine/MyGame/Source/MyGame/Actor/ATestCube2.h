// 05.24
// 비동기 에셋 로드를 확인하기 위해 만든 테스트용 큐브액터 2

#pragma once
#include "Engine/GameFramework/AActor.h"

class UStaticMeshComponent;

class ATestCube2 : public AActor
{
	MY_GENERATE_BODY(ATestCube2)

	ATestCube2();

	void Tick(float DeltaSeconds) override;

protected:
	std::shared_ptr<UStaticMeshComponent> SM0;
	std::shared_ptr<UStaticMeshComponent> SM1;
	std::shared_ptr<UStaticMeshComponent> SM2;
	std::shared_ptr<UStaticMeshComponent> SM3;
	std::shared_ptr<UStaticMeshComponent> SM4;
	std::shared_ptr<UStaticMeshComponent> SM5;
	std::shared_ptr<UStaticMeshComponent> SM6;
	std::shared_ptr<UStaticMeshComponent> SM7;
	std::shared_ptr<UStaticMeshComponent> SM8;
	std::shared_ptr<UStaticMeshComponent> SM9;
	std::shared_ptr<UStaticMeshComponent> SM10;
	std::shared_ptr<UStaticMeshComponent> SM11;
	std::shared_ptr<UStaticMeshComponent> SM12;
	std::shared_ptr<UStaticMeshComponent> SM13;
	std::shared_ptr<UStaticMeshComponent> SM14;

private:
	

};
