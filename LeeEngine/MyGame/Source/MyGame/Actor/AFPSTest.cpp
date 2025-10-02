#include "CoreMinimal.h"
#include "AFPSTest.h"

AFPSTest::AFPSTest()
{
	SM_Attacker = std::make_shared<UStaticMeshComponent>();
	SM_Attacker->SetupAttachment(GetRootComponent());
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

void AFPSTest::Timer_Repeat()
{
	static int a = 0;
	
	MY_LOG("Timer",EDebugLogLevel::DLL_Display, "Repeat -> " +  std::to_string(a++));
}
void AFPSTest::Timer_OneTime()
{
	MY_LOG("Timer",EDebugLogLevel::DLL_Display, "OneTime");
}

void AFPSTest::BeginPlay()
{
	AActor::BeginPlay();


	SM_Attacker->SetCollisionObjectType(ECollisionChannel::Enemy);
	SM_Attacker->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	//GEngine->GetTimerManager()->SetTimer(TestHandle_OneTime, {this, &AFPSTest::Timer_OneTime}, 5.0f);
	//GEngine->GetTimerManager()->SetTimer(TestHandle_Repeat, {this, &AFPSTest::Timer_Repeat}, 0.0f, true, 0.5f);
}

void AFPSTest::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
	CurTime += DeltaSeconds;
	if (CurTime >= ShotDelay)
	{
		XMFLOAT3 Start = GetActorLocation();
		XMFLOAT3 End = Start + XMFLOAT3{0,0,-600};
		FHitResult HitResult;

		std::vector<ECollisionChannel> Channel;
		Channel.reserve(static_cast<UINT>(ECollisionChannel::Count));
		Channel.emplace_back(ECollisionChannel::Player);
		
		bool bHit = GPhysicsEngine->LineTraceSingleByChannel(Start, End, Channel, HitResult, 0.05f);
		if (bHit)
		{
			HitResult.HitActor->TakeDamage(100, {"TestShot"}, this);
		}
		CurTime -= ShotDelay;
	}

	MoveTime += DeltaSeconds;
	if (0<=MoveTime && MoveTime <= 2.0f)
	{
		SetActorLocation(MyMath::Lerp({100,100,100}, {600,100,100}, MoveTime/2));
	}
	else if (2.0f <= MoveTime && MoveTime <= 4.0f)
	{
		SetActorLocation(MyMath::Lerp({600,100,100}, {100,100,100}, (MoveTime-2.0f)/2));
	}
	else
	{
		MoveTime = 0.0f;
	}
}
