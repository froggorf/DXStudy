#include "CoreMinimal.h"
#include "AGideonFireBall.h"

AGideonFireBall::AGideonFireBall()
{
	SM_FireBall = std::make_shared<UStaticMeshComponent>();
	SM_FireBall->SetupAttachment(GetRootComponent());

	// 어차피 정해진 위치에 가면 터지는 액터이므로 굳이 콜리젼 설정 x
	SM_FireBall->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// 자주 스폰되었다 사라질 수 있는 액터이므로 미리 만들어놓고 사용
	// TODO: 된다면 오브젝트 풀링을 사용하는 방안도 생각해보기
	AssetManager::GetAsyncAssetCache("SM_Box",[this](std::shared_ptr<UObject> Object)
		{
			SM_FireBall->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
			
		});
	SM_FireBall->SetRelativeScale3D({10,10,10});
}

void AGideonFireBall::Register()
{
	AActor::Register();

	
}

void AGideonFireBall::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	//CurrentTime += DeltaSeconds;
	//SetActorLocation(MyMath::Lerp(StartPosition, TargetPosition, CurrentTime / ThrowTime));
	//if (CurrentTime >= ThrowTime)
	//{
	//	Explosion();
	//}
}

void AGideonFireBall::Initialize(AGideonCharacter* Spawner, const XMFLOAT3& TargetPosition, const FAttackData& AttackData)
{
	this->Spawner = Spawner;
	this->TargetPosition = TargetPosition;
	ExplosionAttackData = AttackData;
	StartPosition = GetActorLocation();
}

void AGideonFireBall::Explosion()
{
	ExplosionAttackData.bIsAttackCenterFixed = true;
	ExplosionAttackData.AttackCenterPos = GetActorLocation();
	Spawner->ApplyDamageToEnemy(ExplosionAttackData, "FireBall");

	DestroySelf();
}
