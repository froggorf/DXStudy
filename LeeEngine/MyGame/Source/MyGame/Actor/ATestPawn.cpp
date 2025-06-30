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
	
}

void ATestPawn::BeginPlay()
{
	AActor::BeginPlay();

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

	
	
	static float time = 0;
	time += DeltaSeconds;
	if (time > 1.0f/60)
	{
		time -= 1.0f/60;
		FTransform Transform = SKComp->GetSocketTransform("spine_01");
		GEngine->GetWorld()->DrawDebugBox(Transform.Translation, {5,5,5},{1,0,0},XMVectorSet(0,0,0,1), 1.0f/60+0.01f);

		Transform = SKComp->GetSocketTransform("index_03_l");
		GEngine->GetWorld()->DrawDebugBox(Transform.Translation, {5,5,5},{1,1,0},XMVectorSet(0,0,0,1), 1.0f/60+0.01f);

		Transform = SKComp->GetSocketTransform("upperarm_l");
		GEngine->GetWorld()->DrawDebugBox(Transform.Translation, {5,5,5},{1,0,1},XMVectorSet(0,0,0,1), 1.0f/60+0.01f);

		Transform = SKComp->GetSocketTransform("foot_l");
		GEngine->GetWorld()->DrawDebugBox(Transform.Translation, {5,5,5},{0,1,0},XMVectorSet(0,0,0,1), 1.0f/60+0.01f);
	}

	
}
