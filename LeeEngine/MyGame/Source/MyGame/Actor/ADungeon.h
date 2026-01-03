// 05.24
// 비동기 에셋 로드를 확인하기 위해 만든 테스트용 큐브액터 2

#pragma once
#include "Engine/Class/Framework/ACharacter.h"


class ADungeon : public AActor
{
	MY_GENERATE_BODY(ADungeon)
	
	ADungeon();
	void Register() override;

public:

protected:
	std::shared_ptr<UStaticMeshComponent> Water;
	std::shared_ptr<UStaticMeshComponent> Ground;
};
