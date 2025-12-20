#include "CoreMinimal.h"
#include "ATownGameMode.h"

#include "Engine/World/UWorld.h"
#include "MyGame/Character/Enemy/AEnemyBase.h"

void ATownGameMode::Tick(float DeltaSeconds)
{
	AGameMode::Tick(DeltaSeconds);

	if (ImGui::IsKeyPressed(ImGuiKey_9))
	{
		const std::shared_ptr<AActor>& Actor = GetWorld()->GetPersistentLevel()->FindActorByName("Gideon");
		GetWorld()->GetPlayerController()->OnPossess(std::dynamic_pointer_cast<ACharacter>(Actor).get());
	}
	if (ImGui::IsKeyPressed(ImGuiKey_0))
	{
		const std::shared_ptr<AActor>& Actor = GetWorld()->GetPersistentLevel()->FindActorByName("Sanhwa");
		GetWorld()->GetPlayerController()->OnPossess(std::dynamic_pointer_cast<ACharacter>(Actor).get());
	}
}

void ATownGameMode::BeginPlay()
{
	AGameMode::BeginPlay();

	const std::shared_ptr<AActor>& Gideon = GetWorld()->SpawnActor("AGideonCharacter", FTransform{XMFLOAT3{ -500.0f,0.0f,-500.0f}, XMFLOAT4{0.0f,0.0f,0.0f,1.0f}, {1.0f,1.0f,1.0f}}, "Gideon"); //GetWorld()->GetPersistentLevel()->FindActorByName("MyCharacter");
	GetWorld()->GetPlayerController()->OnPossess(std::dynamic_pointer_cast<ACharacter>(Gideon).get());

	const std::shared_ptr<AActor>& Sanhwa = GetWorld()->SpawnActor("ASanhwaCharacter", FTransform{XMFLOAT3{ -600,0.0f,-500.0f}, XMFLOAT4{0.0f,0.0f,0.0f,1.0f}, XMFLOAT3{1.0f,1.0f,1.0f}}, "Sanhwa");
}

void ATownGameMode::StartGame()
{
	AGameMode::StartGame();
}

void ATownGameMode::EndGame()
{
	AGameMode::EndGame();
}
