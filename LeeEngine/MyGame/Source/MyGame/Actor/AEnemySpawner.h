// 05.24
// 비동기 에셋 로드를 확인하기 위해 만든 테스트용 큐브액터 2

#pragma once
#include "Engine/GameFramework/AActor.h"


class UStaticMeshComponent;

class AEnemySpawner : public AActor
{
	MY_GENERATE_BODY(AEnemySpawner)
	
	AEnemySpawner();
	void Register() override;

public:

protected:
	std::shared_ptr<UStaticMeshComponent> Arrow;
};
