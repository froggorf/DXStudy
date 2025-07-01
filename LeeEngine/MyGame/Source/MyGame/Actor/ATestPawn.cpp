#include "CoreMinimal.h"
#include "ATestPawn.h"

#include "Engine/World/UWorld.h"

ATestPawn::ATestPawn()
{

	SKComp = std::make_shared<USkeletalMeshComponent>();
	SKComp->SetupAttachment(GetRootComponent());
	AssetManager::GetAsyncAssetCache("SK_MyUEFN",[this](std::shared_ptr<UObject> Object)
		{
			SKComp->SetSkeletalMesh(std::static_pointer_cast<USkeletalMesh>(Object));
		});
	SKComp->SetAnimInstanceClass("UMyAnimInstance");

	SMSword = std::make_shared<UStaticMeshComponent>();
	SMSword->SetupAttachment(SKComp, "hand_r");
	AssetManager::GetAsyncAssetCache("SM_Sword", [this](std::shared_ptr<UObject> Object)
	{
			SMSword->SetStaticMesh(std::static_pointer_cast<UStaticMesh>(Object));
	});
	SMSword->SetRelativeScale3D({0.2f,0.2f,0.2f});
	SMSword->SetRelativeLocation({-9.895, -3.056, -6.95});
	SMSword->SetRelativeRotation(XMFLOAT4{0.073,0.09,-0.638,0.761});

	CapsuleComp = std::make_shared<UCapsuleComponent>();
	CapsuleComp->SetupAttachment(SMSword);
	CapsuleComp->SetHalfHeight(250);
	CapsuleComp->SetRelativeRotation(XMFLOAT3{90,0,0});
	CapsuleComp->SetRelativeLocation({0.0f,0.0f,-350.0f});
	CapsuleComp->SetRadius(25);
	

	UTestComponent* NewTestComp = dynamic_cast<UTestComponent*>( CreateDefaultSubobject("TestActorComp", "UTestComponent"));
	if(NewTestComp)
	{
		TestComp = std::make_shared<UTestComponent>(*NewTestComp);	
	}
	
}

void ATestPawn::BeginPlay()
{
	AActor::BeginPlay();
	SMSword->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SMSword->GetBodyInstance()->OnComponentBeginOverlap.Add(this, &ATestPawn::AttackStart);
	SMSword->GetBodyInstance()->SetResponseToChannel(ECollisionChannel::WorldStatic, ECollisionResponse::Block);
	SMSword->GetBodyInstance()->SetDebugDraw(true);
	//CapsuleComp->SetResponseToChannel(ECollisionChannel::WorldStatic, ECollisionResponse::Overlap);
	//CapsuleComp->SetDebugDraw(true);
	//CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//CapsuleComp->OnComponentBeginOverlap.Add(this, &ATestPawn::AttackStart);
	//CapsuleComp->OnComponentEndOverlap.Add(this, &ATestPawn::AttackEnd);
}

void ATestPawn::AttackStart(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp)
{
	MY_LOG("Begin",EDebugLogLevel::DLL_Warning, OtherActor->GetName());
}
void ATestPawn::AttackEnd(UShapeComponent* OverlappedComponent, AActor* OtherActor, UShapeComponent* OtherComp)
{

	MY_LOG("End",EDebugLogLevel::DLL_Warning, OtherActor->GetName());
}



void ATestPawn::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	const float power = 3;
	if (ImGui::IsKeyDown(ImGuiKey_I))
	{
		SMSword->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::Physics);
	}
	/*
	if (ImGui::IsKeyDown(ImGuiKey_K))
	{
		CapsuleComp->AddForce({0,0,-power});
	}
	if (ImGui::IsKeyDown(ImGuiKey_J))
	{
		CapsuleComp->AddForce({-power,0,0});
	}
	if (ImGui::IsKeyDown(ImGuiKey_L))
	{
		CapsuleComp->AddForce({power,0,0});
	}

	if (ImGui::IsKeyDown(ImGuiKey_Space))
	{
		CapsuleComp->AddForce({0,power,0});
	}*/

}
