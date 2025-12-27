#include "CoreMinimal.h"
#include "ATownGameMode.h"

#include "Engine/World/UWorld.h"
#include "MyGame/Character/Enemy/AEnemyBase.h"
#include "MyGame/Character/Player/AGideonCharacter.h"
#include "MyGame/Character/Player/ASanhwaCharacter.h"

std::array<int, static_cast<int>(EEquipType::Count)> ATownGameMode::EquipLevel;

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

	static bool Test = false;
	if (!Test && ImGui::IsKeyPressed(ImGuiKey_7))
	{
		Test = true;
		GEngine->ChangeLevelByName("TestLevel");
	}

	//if (ImGui::IsKeyPressed(ImGuiKey_C))
	//{
	//	GEngine->ChangeLevelByName("");
	//}
}

void ATownGameMode::BeginPlay()
{
	AGameMode::BeginPlay();

	// TODO: 데이터 로드 하기
	EquipLevel = {0,0,0,0};

	// TODO: 데이터 셋해주기
	const std::shared_ptr<AGideonCharacter>& Gideon = std::dynamic_pointer_cast<AGideonCharacter>(
		GetWorld()->SpawnActor("AGideonCharacter", FTransform{XMFLOAT3{ -500.0f,0.0f,-500.0f}, XMFLOAT4{0.0f,0.0f,0.0f,1.0f}, {1.0f,1.0f,1.0f}}, "Gideon")
	);
	GetWorld()->GetPlayerController()->OnPossess(Gideon.get());
	Gideon->SetEquipmentLevel(EquipLevel);

	const std::shared_ptr<ASanhwaCharacter>& Sanhwa = std::dynamic_pointer_cast<ASanhwaCharacter>(
		GetWorld()->SpawnActor("ASanhwaCharacter", FTransform{XMFLOAT3{ -600,0.0f,-500.0f}, XMFLOAT4{0.0f,0.0f,0.0f,1.0f}, XMFLOAT3{1.0f,1.0f,1.0f}}, "Sanhwa")
	);
	Sanhwa->SetEquipmentLevel(EquipLevel);
	
}

void ATownGameMode::StartGame()
{
	AGameMode::StartGame();
}

void ATownGameMode::EndGame()
{
	AGameMode::EndGame();
}

void ATownGameMode::AddEquipLevel(EEquipType Type)
{
	++EquipLevel[static_cast<int>(Type)];
	// TODO: 데베 쓰기
	// 강화 비용 같은거도 적용하기

	MY_LOG("LOG",EDebugLogLevel::DLL_Warning, std::to_string(static_cast<int>(Type)) + " Enchant Success");

	if (const std::shared_ptr<ASanhwaCharacter>& Sanhwa = SanhwaCharacter.lock())
	{
		Sanhwa->SetEquipmentLevel(EquipLevel);
	}
	if (const std::shared_ptr<AGideonCharacter>& Gideon = GideonCharacter.lock())
	{
		Gideon->SetEquipmentLevel(EquipLevel);
	}
}
