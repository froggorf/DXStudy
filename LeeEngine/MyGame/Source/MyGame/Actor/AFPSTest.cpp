#include "CoreMinimal.h"
#include "AFPSTest.h"

AFPSTest::AFPSTest()
{
	SM_Attacker = std::make_shared<UStaticMeshComponent>();
	SM_Attacker->SetupAttachment(GetRootComponent());
	SM_Attacker->SetRelativeLocation({100,100,100});
	SM_Attacker->SetRelativeScale3D({10,10,10});
	AssetManager::GetAsyncAssetCache("SM_DeferredSphere",[this](std::shared_ptr<UObject> Object)
		{
			SM_Attacker->SetStaticMesh(std::dynamic_pointer_cast<UStaticMesh>(Object));
		});

	ShotDelay = 0.05f;
}

void AFPSTest::Register()
{
	AActor::Register();
}

void AFPSTest::BeginPlay()
{
	AActor::BeginPlay();
	SM_Attacker->SetCollisionObjectType(ECollisionChannel::Camera);
	
}

void AFPSTest::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
	CurTime += DeltaSeconds;
	if (CurTime >= ShotDelay)
	{
		XMFLOAT3 Start = {100, 100, 100};
		XMFLOAT3 End = {100,100,-500};
		FHitResult HitResult;

		std::vector<ECollisionChannel> Channel;
		Channel.reserve(static_cast<UINT>(ECollisionChannel::Count));
		Channel.emplace_back(ECollisionChannel::Pawn);
		
		bool bHit = gPhysicsEngine->LineTraceSingleByChannel(Start, End, Channel, HitResult, 0.05f);
		if (bHit)
		{
			HitResult.HitActor->TakeDamage(100, {"TestShot"}, this);
		}
		CurTime -= ShotDelay;
	}
}
