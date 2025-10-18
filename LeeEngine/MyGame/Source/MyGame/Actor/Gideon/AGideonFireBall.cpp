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

void AGideonFireBall::BeginPlay()
{
	AActor::BeginPlay();

	GEngine->GetTimerManager()->SetTimer(ThrowTimerHandle, {this, &AGideonFireBall::FlyFireBall}, 0.0f, true, ThrowTimerTickTime);
	SM_FireBall->OnComponentHit.Add(this, &AGideonFireBall::OnHit);
}

void AGideonFireBall::Initialize(AGideonCharacter* Spawner, const XMFLOAT3& TargetPosition, const FAttackData& AttackData)
{
	this->Spawner = Spawner;
	this->TargetPosition = TargetPosition;
	ExplosionAttackData = AttackData;
	StartPosition = GetActorLocation();
}

void AGideonFireBall::FlyFireBall()
{
	// 타이머 틱 당 갈수있는 unit 거리
	float ThrowUnitPerTimerTick = SpeedPerSecond * ThrowTimerTickTime;
	float CurrentRemainDistance = MyMath::GetDistance(GetActorLocation(), TargetPosition);
	// 지금 틱 만에 갈 수 있다면
	if (ThrowUnitPerTimerTick >= CurrentRemainDistance)
	{
		SetActorLocation(TargetPosition);
		Explosion();
		GEngine->GetTimerManager()->ClearTimer(ThrowTimerHandle);
		return;
	}

	XMFLOAT3 Direction = MyMath::GetDirectionUnitVector(GetActorLocation(), TargetPosition);
	SetActorLocation(GetActorLocation() + Direction * ThrowUnitPerTimerTick);
}

void AGideonFireBall::OnHit(UShapeComponent* ShapeComp, AActor* HitActor, UShapeComponent* HitComponent, const FHitResult& Result)
{
	MY_LOG("AGideonFireBall", EDebugLogLevel::DLL_Warning, "OnComponentHit");
}

void AGideonFireBall::Explosion()
{
	ExplosionAttackData.bIsAttackCenterFixed = true;
	ExplosionAttackData.AttackCenterPos = GetActorLocation();
	Spawner->ApplyDamageToEnemy(ExplosionAttackData, "FireBall");

	DestroySelf();
}
