// 05.24
// 비동기 에셋 로드를 확인하기 위해 만든 테스트용 큐브액터 2

#pragma once
#include "Engine/Class/Framework/ACharacter.h"


class ACoin : public AActor
{
	MY_GENERATE_BODY(ACoin)
	
	ACoin();
	void Register() override;
	void SetupCoinCollision();
	void OverlapToPlayer();

	void BeginPlay() override;
	void Tick(float DeltaSeconds) override;
public:

protected:
	std::shared_ptr<UStaticMeshComponent> Coin;
	float Time = 0.0f;
	bool bGoToPlayer = false;
	XMFLOAT3 OriginPos;
};
