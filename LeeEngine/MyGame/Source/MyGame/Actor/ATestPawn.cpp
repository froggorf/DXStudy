#include "CoreMinimal.h"
#include "ATestPawn.h"

#include "Engine/World/UWorld.h"

ATestPawn::ATestPawn()
{
	//CapsuleComp = std::make_shared<UCapsuleComponent>();
	//CapsuleComp->SetupAttachment(GetRootComponent());
	//CapsuleComp->SetHalfHeight(10);
	//CapsuleComp->SetRadius(3);
	//CapsuleComp->SetRelativeLocation(XMFLOAT3{70.0f,00.0f,0.0f});

	SKComp = std::make_shared<USkeletalMeshComponent>();
	SKComp->SetupAttachment(GetRootComponent());
	AssetManager::GetAsyncAssetCache("SK_MyUEFN",[this](std::shared_ptr<UObject> Object)
		{
			SKComp->SetSkeletalMesh(std::static_pointer_cast<USkeletalMesh>(Object));
		});
	SKComp->SetAnimInstanceClass("UMyAnimInstance");

	SMSword = std::make_shared<UStaticMeshComponent>();
	SMSword->SetupAttachment(SKComp, "hand_l");
	AssetManager::GetAsyncAssetCache("SM_Sword", [this](std::shared_ptr<UObject> Object)
	{
			SMSword->SetStaticMesh(std::static_pointer_cast<UStaticMesh>(Object));
	});
	SMSword->SetRelativeScale3D({0.2f,0.2f,0.2f});
	SMSword->SetRelativeLocation({10.218,4.3,2.225});
	SMSword->SetRelativeRotation(XMFLOAT4{-0.614,0.752,0.222,0.093});
	

	UTestComponent* NewTestComp = dynamic_cast<UTestComponent*>( CreateDefaultSubobject("TestActorComp", "UTestComponent"));
	if(NewTestComp)
	{
		TestComp = std::make_shared<UTestComponent>(*NewTestComp);	
	}
	
}

void ATestPawn::BeginPlay()
{
	AActor::BeginPlay();
	SMSword->GetBodyInstance()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//CapsuleComp->SetResponseToChannel(ECollisionChannel::WorldStatic, ECollisionResponse::Block);
	//CapsuleComp->SetResponseToChannel(ECollisionChannel::WorldDynamic, ECollisionResponse::Ignore);
	//CapsuleComp->SetCollisionEnabled(ECollisionEnabled::Physics);
	//CapsuleComp->SetCollisionObjectType(ECollisionChannel::Pawn);
}

void ATestPawn::Tick(float DeltaSeconds)
{
	AActor::Tick(DeltaSeconds);

	const float power = 3;
	/*if (ImGui::IsKeyDown(ImGuiKey_I))
	{
		CapsuleComp->AddForce({0,0,power});
	}
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
