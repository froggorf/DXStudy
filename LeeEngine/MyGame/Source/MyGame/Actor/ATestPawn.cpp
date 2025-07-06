#include "CoreMinimal.h"
#include "ATestPawn.h"

#include "ATestCube2.h"
#include "Engine/World/UWorld.h"

ATestPawn::ATestPawn()
{
	Radius = 20.0f;
	HalfHeight = 65.0f;
	CapsuleComp = std::make_shared<UCapsuleComponent>();
	CapsuleComp->SetRadius(Radius);
	CapsuleComp->SetHalfHeight(HalfHeight);
	SetRootComponent(CapsuleComp);
	


	SKComp = std::make_shared<USkeletalMeshComponent>();
	SKComp->SetupAttachment(GetRootComponent());
	AssetManager::GetAsyncAssetCache("SK_MyUEFN",[this](std::shared_ptr<UObject> Object)
		{
			SKComp->SetSkeletalMesh(std::static_pointer_cast<USkeletalMesh>(Object));
		});
	SKComp->SetAnimInstanceClass("UMyAnimInstance");
	SKComp->SetRelativeLocation({0,-85,0});

	SMSword = std::make_shared<UStaticMeshComponent>();
	SMSword->SetupAttachment(SKComp, "hand_r");
	AssetManager::GetAsyncAssetCache("SM_Sword", [this](std::shared_ptr<UObject> Object)
	{
			SMSword->SetStaticMesh(std::static_pointer_cast<UStaticMesh>(Object));
	});
	SMSword->SetRelativeScale3D({0.2f,0.2f,0.2f});
	SMSword->SetRelativeLocation({-9.895, -3.056, -6.95});
	SMSword->SetRelativeRotation(XMFLOAT4{0.073,0.09,-0.638,0.761});

	

	UTestComponent* NewTestComp = dynamic_cast<UTestComponent*>( CreateDefaultSubobject("TestActorComp", "UTestComponent"));
	if(NewTestComp)
	{
		TestComp = std::make_shared<UTestComponent>(*NewTestComp);	
	}

	
	
}

void ATestPawn::BeginPlay()
{
	AActor::BeginPlay();

	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CapsuleComp->SetSimulatePhysics(true);
	CapsuleComp->SetKinematicRigidBody(true);
	CapsuleComp->SetDebugDraw(true);
	CapsuleComp->SetupAttachment(GetRootComponent());

	SMSword->GetBodyInstance()->OnComponentBeginOverlap.Add(this, &ATestPawn::AttackStart);
	SMSword->GetBodyInstance()->OnComponentHit.Add(this, &ATestPawn::OnComponentHitEvent);


	SMSword->GetBodyInstance()->SetSimulatePhysics(false);
	SMSword->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	for (UINT i = 0; i < static_cast<UINT>(ECollisionChannel::Count); ++i)
	{
		SMSword->GetBodyInstance()->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(i), ECollisionResponse::Overlap);
	}
	SMSword->GetBodyInstance()->SetObjectType(ECollisionChannel::Pawn);

	Manager = PxCreateControllerManager(*gPhysicsEngine->GetScene());
	physx::PxCapsuleControllerDesc desc;
	desc.height = HalfHeight*2;
	desc.radius = Radius;
	desc.position = physx::PxExtendedVec3(0,0,0);
	desc.material = gPhysicsEngine->GetDefaultMaterial();
	desc.stepOffset = 10.0f;

	Controller = Manager->createController(desc);
}

void ATestPawn::OnComponentHitEvent(UShapeComponent* HitComponent, AActor* OtherActor, UShapeComponent* OtherComp, const FHitResult& HitResults)
{
	MY_LOG("Hit",EDebugLogLevel::DLL_Warning, OtherActor->GetName() + " -> " + OtherComp->GetName());
}


void ATestPawn::AttackStart(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp)
{
	if (ATestCube2* TestCube = dynamic_cast<ATestCube2*>(OtherActor))
	{
		TestCube->TestCompFunc();
		
	}
}
void ATestPawn::AttackEnd(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp)
{

	MY_LOG("End",EDebugLogLevel::DLL_Warning, OtherActor->GetName());
}

void ATestPawn::SetAttackStart()
{
	SMSword->GetBodyInstance()->SetSimulatePhysics(true);
	SMSword->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::Physics);
	SMSword->GetBodyInstance()->SetKinematicRigidBody(true);
	SMSword->GetBodyInstance()->SetDebugDraw(true);
}

void ATestPawn::SetAttackEnd()
{
	SMSword->GetBodyInstance()->SetSimulatePhysics(false);
	SMSword->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SMSword->GetBodyInstance()->SetDebugDraw(false);
}


void ATestPawn::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);
	const float MaxSpeed = 1000.0f;
	if (ImGui::IsKeyDown(ImGuiKey_I))
	{
		physx::PxVec3 Movement = {1,0,0};
		Movement *= MaxSpeed;
		physx::PxControllerFilters Filters;
		Controller->move(Movement*DeltaSeconds,0.01f, DeltaSeconds,Filters);
		physx::PxExtendedVec3 NewPos = Controller->getPosition();
		XMFLOAT3 NewP = {static_cast<float>(NewPos.x),static_cast<float>(NewPos.y),static_cast<float>(-NewPos.z)};
		CapsuleComp->SetWorldLocation(NewP);
	}
	
	
}
