#include "CoreMinimal.h"
#include "ATestGameMode.h"

#include "Engine/World/UWorld.h"
#include "MyGame/Character/Enemy/AEnemyBase.h"

void ATestGameMode::StartGame()
{
	AGameMode::StartGame();

	FTransform SpawnTransform{XMFLOAT3{500,150,500}, MyMath::ForwardVectorToRotationQuaternion({-1,0,-1}), XMFLOAT3{1,1,1}};
	BossActor = std::dynamic_pointer_cast<ADragon>(GetWorld()->SpawnActor("ADragon", SpawnTransform));
}

void ATestGameMode::EndGame()
{
	std::shared_ptr<ADragon> Dragon = BossActor.lock();
	if (Dragon)
	{
		GetWorld()->GetPersistentLevel()->DestroyActor(Dragon.get());
	}
	
	AGameMode::EndGame();
}
